#include "crosslib/watch.h"
#include "crosslib/string.h"
#include "crosslib/thread.h"
#include <Windows.h>

// TODO: should rework this whole thing to use a single watching thread, w/ WaitForMultipleObjects
//       changeHookCreate adds to list; changeHookFree removes from list, joining thread once empty

// amount of time change hooks will block while waiting for change events
// - smaller values will return faster from clWatch_changeHookFree(), but eat more cpu time
// - larger values will spend more time idle, but will take longer to return from clWatch_changeHookFree()
#define CHANGE_HOOK_INTERVAL 250

typedef struct struct_ChangeHookPlatformData {
	HANDLE                 changeHandle;
	clThread_ThreadHandle  threadHandle;
	clWatch_ChangeFunction callback;
} ChangeHookPlatformData;

void changeHookProxy(void* data) {
	clWatch_ChangeHook* hook = (clWatch_ChangeHook*)data;
	ChangeHookPlatformData* platformData = (ChangeHookPlatformData*)hook->platformData;
	DWORD waitStatus;

	while(true) {
		if(!platformData->callback) { break; }

		waitStatus = WaitForSingleObject(platformData->changeHandle, CHANGE_HOOK_INTERVAL);
		if((waitStatus != WAIT_TIMEOUT) && (waitStatus != WAIT_FAILED)) {
			platformData->callback(hook);
		}
		if(!FindNextChangeNotification(platformData->changeHandle)) { break; }
	}

	FindCloseChangeNotification(platformData->changeHandle);
	clThread_threadReturn();
}

bool clWatch_changeHookCreate(char* path, clWatch_ChangeFunction callback, void* userData, clWatch_ChangeHook** hook) {
	char16_t* wpath = clString_utf16from8(path);

	HANDLE changeHandle = FindFirstChangeNotificationW(wpath, FALSE,
		FILE_NOTIFY_CHANGE_FILE_NAME  |
		FILE_NOTIFY_CHANGE_DIR_NAME   |
		FILE_NOTIFY_CHANGE_ATTRIBUTES |
		FILE_NOTIFY_CHANGE_SIZE       |
		FILE_NOTIFY_CHANGE_LAST_WRITE
	);

	free(wpath);
	if(changeHandle == INVALID_HANDLE_VALUE) { return false; }

	ChangeHookPlatformData* platformData = malloc(sizeof(ChangeHookPlatformData));
	platformData->changeHandle = changeHandle;
	platformData->threadHandle = NULL;
	platformData->callback     = callback;

	*hook = malloc(sizeof(clWatch_ChangeHook));
	(*hook)->path         = _strdup(path);
	(*hook)->userData     = userData;
	(*hook)->platformData = (void*)platformData;

	bool success = clThread_threadCreate(changeHookProxy, *hook, &(platformData->threadHandle));
	if(!success) {
		clWatch_changeHookFree(*hook);
		return false;
	}

	return true;
}

void clWatch_changeHookFree(clWatch_ChangeHook* hook) {
	ChangeHookPlatformData* platformData = (ChangeHookPlatformData*)hook->platformData;

	if(platformData->threadHandle) {
		platformData->callback = NULL;
		clThread_threadWait(platformData->threadHandle);
		clThread_threadFree(platformData->threadHandle);
	} else {
		FindCloseChangeNotification(platformData->changeHandle);
	}

	free(hook->platformData);
	free(hook->path);
	free(hook);
}
