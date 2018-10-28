#include "crosslib/string.h"
#include <Windows.h>

char* clString_utf8from16(char16_t* utf16) {
	int buffSize = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, NULL, NULL);
	if(!buffSize) { return NULL; }

	char* buffer = malloc(buffSize + 1);
	int result = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, buffer, buffSize, NULL, NULL);
	if(!result) { free(buffer); return NULL; }
	buffer[buffSize] = 0x00;

	return buffer;
}

char16_t* clString_utf16from8(char* utf8) {
	int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	if(!wideSize) { return NULL; }

	char16_t* buffer = malloc(sizeof(char16_t) * (wideSize + 1));
	int result = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, buffer, wideSize);
	if(!result) { free(buffer); return NULL; }
	buffer[wideSize] = 0x00;

	return buffer;
}
