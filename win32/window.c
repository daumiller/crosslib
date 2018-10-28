#include "crosslib/window.h"
#include "crosslib/string.h"
#include <Windows.h>

//==============================================================================
// ACTIVE WINDOW FUNCTIONS
//==============================================================================
static bool activeHookRunning = false;
static HWINEVENTHOOK activeHookHandle = NULL;
static clWindow_ActiveWindowCallback activeHookCallback = NULL;

#define MAX_BUFFER_LENGTH 1024
bool clWindow_activeWindowRead(clWindow_ActiveWindow* window) {
	HWND foreground = GetForegroundWindow();
	if(!foreground) { return false; }

	static wchar_t buffer[MAX_BUFFER_LENGTH];
	GetWindowTextW(foreground, buffer, MAX_BUFFER_LENGTH); // can't check return, window could really be titleless
	char* title = clString_utf8from16(buffer);

	DWORD pid = 0;
	if(!GetWindowThreadProcessId(foreground, &pid)) { free(title); return false; }

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
	if(!hProcess) { free(title); return false; }

	DWORD characters = MAX_BUFFER_LENGTH;
	BOOL readExecutable = QueryFullProcessImageNameW(hProcess, 0, buffer, &characters);
	if(!readExecutable) { free(title); return false; }
	char* executable = clString_utf8from16(buffer);

	window->title   = title;
	window->program = executable;
	window->pid     = (int64_t)pid;
	return true;
}

void clWindow_activeWindowFree(clWindow_ActiveWindow* window) {
	if(window->title)   { free(window->title);   window->title   = NULL; }
	if(window->program) { free(window->program); window->program = NULL; }
}

static void activeHookProxy(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime) {
	if((event != EVENT_SYSTEM_FOREGROUND) || (idObject != OBJID_WINDOW) || (idChild != CHILDID_SELF) || !hwnd || !activeHookCallback) { return; }

	static clWindow_ActiveWindow activeWindow;
	if(clWindow_activeWindowRead(&activeWindow)) {
		activeHookCallback(&activeWindow);
		clWindow_activeWindowFree(&activeWindow);
	}

	UNREFERENCED_PARAMETER(hook);
	UNREFERENCED_PARAMETER(idEventThread);
	UNREFERENCED_PARAMETER(dwmsEventTime);
}

bool clWindow_activeHookStart(clWindow_ActiveWindowCallback callback) {
	if(activeHookRunning) { return false; }
	if(!callback) { return false; }

	HWINEVENTHOOK handle = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, (WINEVENTPROC)activeHookProxy, 0, 0, WINEVENT_OUTOFCONTEXT);
	if(handle == 0) { return false; }

	activeHookCallback = callback;
	activeHookHandle   = handle;
	activeHookRunning  = true;

	return true;
}

bool clWindow_activeHookRunning(void) {
	return activeHookRunning;
}

bool clWindow_activeHookStop(void) {
	if(!activeHookHandle) { return false; }
	if(!UnhookWinEvent(activeHookHandle)) { return false; }

	activeHookRunning  = false;
	activeHookHandle   = NULL;
	activeHookCallback = NULL;

	return true;
}

//==============================================================================
// GUI MAIN LOOP FUNCTIONS
//==============================================================================
static HWND hwndGuiLoop = NULL;

bool clWindow_guiLoopBegin(void* guiData) {
	HINSTANCE hInst = *((HINSTANCE*)guiData);

	WNDCLASSA classMain;
	classMain.style         = 0;
	classMain.lpfnWndProc   = DefWindowProcA;
	classMain.cbClsExtra    = 0;
	classMain.cbWndExtra    = 0;
	classMain.hInstance     = hInst;
	classMain.hIcon         = NULL;
	classMain.hCursor       = NULL;
	classMain.hbrBackground = NULL;
	classMain.lpszMenuName  = NULL;
	classMain.lpszClassName = "crosslib-gui-loop-class";
	if(RegisterClassA(&classMain) == 0) { return false; }

	hwndGuiLoop = CreateWindowExA(0, "crosslib-gui-loop-class", "crosslib", WS_DISABLED, 0, 0, 32, 32, NULL, NULL, hInst, NULL);
	if(hwndGuiLoop == NULL) { return false; }

	MSG message;
	while(GetMessageA(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return true;
}

void clWindow_guiLoopStop(void) {
	if(hwndGuiLoop) { PostMessage(hwndGuiLoop, WM_QUIT, 0, 0); }
	hwndGuiLoop = NULL;
}
