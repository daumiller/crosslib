#include "crosslib/process.h"
#include "crosslib/string.h"
#include <Windows.h>

bool clProcess_runCommand(char* path, uint32_t argumentCount, char** argumentValues, char* workingPath) {
	char16_t* wpath = clString_utf16from8(path);
	
	char16_t* wargs = NULL;
	if(argumentCount) {
		char16_t* wargumentValues = malloc(sizeof(char16_t*) * argumentCount);
		uint32_t wlength = 0;
		for(uint32_t index=0; index<argumentCount; ++index) {
			wargumentValues[index] = clString_utf16from8(argumentValues[index]);
			wlength += wcslen(wargumentValues[index]);
		}
		wargs = malloc(sizeof(char16_t) * (wlength + argumentCount + 1));
		char16_t* position = wargs;
		for(uint32_t index=0; index<argumentCount; ++index) {
			position += wsprintfW(position, L"%S ", wargumentValues[index]);
			free(wargumentValues[index]);
		}
		free(wargumentValues);
		wargs[wlength + argumentCount] = 0x0000;
	}

	char16_t* wdir = NULL;
	if(workingPath) { wdir = clString_utf16from8(workingPath); }

	STARTUPINFO         startupInfo; memset(&startupInfo, 0, sizeof(STARTUPINFO));
	PROCESS_INFORMATION processInfo; memset(&processInfo, 0, sizeof(PROCESS_INFORMATION));
	startupInfo.cb = sizeof(STARTUPINFO);
	BOOL result = CreateProcessW(wpath, wargs, NULL, NULL, FALSE, 0, NULL, wdir, &startupInfo, &processInfo);

	free(wpath);
	if(wargs) { free(wargs); }
	if(wdir ) { free(wdir ); }

	if(result != FALSE) {
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
	return (result != FALSE);
}
