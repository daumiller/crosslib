#include "crosslib/path.h"
#include "crosslib/string.h"
#include <Windows.h>
#include "shlwapi.h"
#include "shlobj.h"
#include <string.h>
#include <stdio.h>

char* clPath_combine(char* left, char* right) {
	char16_t* wleft  = clString_utf16from8(left);
	char16_t* wright = clString_utf16from8(right);

	char16_t wresult[MAX_PATH]; wresult[0] = 0;
	if(!PathCombineW(wresult, wleft, wright)) {
		free(wleft); free(wright); return NULL;
	}

	free(wleft);
	free(wright);
	return clString_utf8from16(wresult);
}

char* clPath_resolve(char* path) {
	char16_t* wpath = clString_utf16from8(path);

	DWORD length = GetFullPathNameW(wpath, 0, NULL, NULL);
	if(length == 0) { free(wpath); return NULL; }
	
	char16_t* buffer = malloc(sizeof(char16_t) * length);
	length = GetFullPathNameW(wpath, length, buffer, NULL);
	if(length == 0) { free(wpath); free(buffer); return NULL; }

	char* result = clString_utf8from16(buffer);
	free(wpath);
	free(buffer);
	return result;
}

char* clPath_getHome(void) {
	char16_t* home16 = NULL;
	HRESULT result = SHGetKnownFolderPath(&FOLDERID_Profile, 0, NULL, &home16);
	if(result != S_OK) { return NULL; }
	char* home8 = clString_utf8from16(home16);
	CoTaskMemFree(home16);
	return home8;
}

char* clPath_getWorking(void) {
	DWORD length = GetCurrentDirectoryW(0, NULL);
	if(length == 0) { return NULL; }
	char16_t* buffer = malloc(sizeof(char16_t) * length);
	length = GetCurrentDirectoryW(length, buffer);
	if(length == 0) { free(buffer); return NULL; }

	char* result = clString_utf8from16(buffer);
	free(buffer);
	return result;
}

bool clPath_isFile(char* path) {
	char16_t* wpath = clString_utf16from8(path);
	DWORD attr = GetFileAttributesW(wpath);
	free(wpath);
	if(attr == INVALID_FILE_ATTRIBUTES) { return false; }
	return ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool clPath_isDirectory(char* path) {
	char16_t* wpath = clString_utf16from8(path);
	DWORD attr = GetFileAttributesW(wpath);
	free(wpath);
	if(attr == INVALID_FILE_ATTRIBUTES) { return false; }
	return ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool clPath_fileRead(char* path, uint8_t** data, uint32_t* size) {
	*data = NULL; *size = 0;
	char16_t* wpath = clString_utf16from8(path);
	FILE* fin = _wfopen(wpath, L"rb");
	free(wpath);
	if(!fin) { return false; }
	fseek(fin, 0, SEEK_END);
	long length = ftell(fin);
	fseek(fin, 0, SEEK_SET);

	uint8_t* buffer = malloc(length);
	long read = 0;
	while(read < length) {
		read += fread(buffer+read, 1, length-read, fin);
	}
	fclose(fin);

	*data = buffer;
	*size = (uint32_t)length;
	return true;
}

bool clPath_fileWrite(char* path, uint8_t* data, uint32_t size) {
	char16_t* wpath = clString_utf16from8(path);
	FILE* fout = _wfopen(wpath, L"w");
	free(wpath);
	if(!fout) { return false; }
	uint32_t wrote = 0;
	while(wrote < size) {
		wrote += fwrite(data+wrote, 1, size-wrote, fout);
	}
	fclose(fout);
	return true;
}

typedef struct struct_clPath_DirEntry {
	char16_t* name;
	struct struct_clPath_DirEntry* next;
} clPath_DirEntry;

bool clPath_directoryListCreate(char* path, clPath_DirectoryList** list) {
	*list = NULL;
	char16_t* wpath = clString_utf16from8(path);
	WIN32_FIND_DATAW entry;
	HANDLE hlist = FindFirstFileW(wpath, &entry);
	free(wpath);
	if(hlist == INVALID_HANDLE_VALUE) { return false; }

	clPath_DirEntry* fileFirst = NULL;
	clPath_DirEntry* fileCurr = NULL;
	clPath_DirEntry* directoryFirst = NULL;
	clPath_DirEntry* directoryCurr = NULL;

	do {
		clPath_DirEntry* dirEntry = malloc(sizeof(clPath_DirEntry));
		dirEntry->name = _wcsdup(entry.cFileName);
		dirEntry->next = NULL;
		if(entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if(!directoryFirst) { directoryFirst = directoryCurr = dirEntry; } else { directoryCurr->next = dirEntry; }
			directoryCurr = dirEntry;
		} else {
			if(!fileFirst) { fileFirst = fileCurr = dirEntry; } else { fileCurr->next = dirEntry; }
			fileCurr = dirEntry;
		}
	} while(FindNextFileW(hlist, &entry));
	FindClose(hlist);

	*list = malloc(sizeof(clPath_DirectoryList));
	(*list)->fileCount = (*list)->directoryCount = 0;
	for(fileCurr = fileFirst; fileCurr; fileCurr = fileCurr->next) { ++((*list)->fileCount); }
	for(directoryCurr = directoryFirst; directoryCurr; directoryCurr = directoryCurr->next) { ++((*list)->directoryCount); }
	
	(*list)->fileNames = malloc(sizeof(char*) * (*list)->fileCount);
	(*list)->directoryCount = malloc(sizeof(char*) * (*list)->directoryCount);
	clPath_DirEntry* currNext;

	fileCurr = fileFirst;
	for(uint32_t index=0; index < (*list)->fileCount; ++index) {
		(*list)->fileNames[index] = clString_utf8from16(fileCurr->name);
		currNext = fileCurr->next;
		free(fileCurr->name);
		free(fileCurr);
		fileCurr = currNext;
	}

	directoryCurr = directoryFirst;
	for(uint32_t index=0; index < (*list)->directoryCount; ++index) {
		(*list)->directoryNames[index] = clString_utf8from16(directoryCurr->name);
		currNext = directoryCurr->next;
		free(directoryCurr->name);
		free(directoryCurr);
		directoryCurr = currNext;
	}

	return true;
}

void clPath_directoryListFree(clPath_DirectoryList* list) {
	for(uint32_t index=0; index<list->fileCount; ++index) { free(list->fileNames[index]); }
	free(list->fileNames);
	for(uint32_t index=0; index<list->directoryCount; ++index) { free(list->directoryNames[index]); }
	free(list->directoryNames);
	free(list);
}
