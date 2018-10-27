#include "crosslib/path.h"
#include "crosslib/string.h"
#include <Windows.h>
#include "shlwapi.h"
#include "shlobj.h"
#include <string.h>
#include <stdio.h>

char* clPath_combine(char* left, char* right) {
	wchar_t* wleft  = clString_utf16from8(left);
	wchar_t* wright = clString_utf16from8(right);

	wchar_t wresult[MAX_PATH]; wresult[0] = 0;
	if(!PathCombineW(wresult, wleft, wright)) {
		free(wleft); free(wright); return NULL;
	}

	free(wleft);
	free(wright);
	return clString_utf8from16(wresult);
}

char* clPath_resolve(char* path) {
	wchar_t* wpath = clString_utf16from8(path);

	DWORD length = GetFullPathNameW(wpath, 0, NULL, NULL);
	if(length == 0) { free(wpath); return NULL; }
	
	wchar_t* buffer = malloc(sizeof(wchar_t) * length);
	length = GetFullPathNameW(wpath, length, buffer, NULL);
	if(length == 0) { free(wpath); free(buffer); return NULL; }

	char* result = clString_utf8from16(buffer);
	free(wpath);
	free(buffer);
	return result;
}

char* clPath_getHome() {
	wchar_t* home16 = NULL;
	HRESULT result = SHGetKnownFolderPath(&FOLDERID_Profile, 0, NULL, &home16);
	if(result != S_OK) { return NULL; }
	char* home8 = clString_utf8from16(home16);
	CoTaskMemFree(home16);
	return home8;
}

char* clPath_getWorking() {
	DWORD length = GetCurrentDirectoryW(0, NULL);
	if(length == 0) { return NULL; }
	wchar_t* buffer = malloc(sizeof(wchar_t) * length);
	length = GetCurrentDirectoryW(length, buffer);
	if(length == 0) { free(buffer); return NULL; }

	char* result = clString_utf8from16(buffer);
	free(buffer);
	return result;
}

bool clPath_isFile(char* path) {
	wchar_t* wpath = clString_utf16from8(path);
	DWORD attr = GetFileAttributesW(wpath);
	free(wpath);
	if(attr == INVALID_FILE_ATTRIBUTES) { return false; }
	return ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool clPath_isDirectory(char* path) {
	wchar_t* wpath = clString_utf16from8(path);
	DWORD attr = GetFileAttributesW(wpath);
	free(wpath);
	if(attr == INVALID_FILE_ATTRIBUTES) { return false; }
	return ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool clPath_fileRead(char* path, uint8_t** data, uint32_t* size) {
	*data = NULL; *size = 0;
	wchar_t* wpath = clString_utf16from8(path);
	FILE* fin = wfopen(wpath, "rb");
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
	wchar_t* wpath = clString_utf16from8(path);
	FILE* fout = fopen(path, "w");
	free(wpath);
	if(!fout) { return false; }
	uint32_t wrote = 0;
	while(wrote < size) {
		wrote += fwrite(data+wrote, 1, size-wrote, fout);
	}
	fclose(fout);
	return true;
}

bool clPath_directoryListCreate(char* path, clPath_DirectoryList** list) {
	return false;
}

void clPath_directoryListFree(clPath_DirectoryList* list) {
}
