#include "crosslib/path.h"
#include "crosslib/string.h"
#import <Foundation/Foundation.h>
#include <string.h>
#include <stdlib.h>

char* clPath_combine(char* left, char* right) {
	NSArray* components = @[ [NSString stringWithUTF8String:left], [NSString stringWithUTF8String:right] ];
	NSString* composite = [NSString pathWithComponents:components];
	return strdup([composite UTF8String]);
}

char* clPath_resolve(char* path) {
	NSString* nsPath = [[NSString stringWithUTF8String:path] stringByStandardizingPath];
	if(nsPath.absolutePath) { return strdup([nsPath UTF8String]); }

	char* workingPath = clPath_getWorking();
	char* resolvedPath = clPath_combine(workingPath, [nsPath UTF8String]);
	free(workingPath);
	return resolvedPath;
}

char* clPath_getHome(void) {
	NSURL* path = [[NSFileManager defaultManager] homeDirectoryForCurrentUser];
	return strdup([path fileSystemRepresentation]);
}

char* clPath_getWorking(void) {
	NSString* path = [[NSFileManager defaultManager] currentDirectoryPath];
	return strdup([path UTF8String]);
}

bool clPath_isFile(char* path) {
	NSString* nsPath = [NSString stringWithUTF8String:path];
	BOOL isDirectory;
	BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath:nsPath isDirectory:&isDirectory];
	return (exists && !isDirectory);
}

bool clPath_isDirectory(char* path) {
	NSString* nsPath = [NSString stringWithUTF8String:path];
	BOOL isDirectory;
	BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath:nsPath isDirectory:&isDirectory];
	return (exists && isDirectory);
}

bool clPath_fileRead(char* path, uint8_t** data, uint32_t* size) {
	FILE* fin = fopen(path, "rb");
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
	FILE* fout = fopen(path, "w");
	if(!fout) { return false; }
	uint32_t wrote = 0;
	while(wrote < size) {
		wrote += fwrite(data+wrote, 1, size-wrote, fout);
	}
	fclose(fout);
	return true;
}

bool clPath_directoryListCreate(char* path, clPath_DirectoryList** list) {
	NSArray* contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[NSString stringWithUTF8String:path] error:NULL];
	if(!contents) { *list = NULL; return false; }
	uint32_t totalCount = [contents count];
	
	*list = malloc(sizeof(clPath_DirectoryList));
	(*list)->fileCount = 0;
	(*list)->directoryCount = 0;

	bool* isDirectory = malloc(sizeof(bool) * totalCount);
	for(uint32_t index=0; index<totalCount; ++index) {
		char* fullPath = clPath_combine(path, [[contents objectAtIndex:index] UTF8String]);
		isDirectory[index] = clPath_isDirectory(fullPath);
		free(fullPath);
		if(isDirectory[index]) { ++((*list)->directoryCount); } else { ++((*list)->fileCount); }
	}	
	(*list)->fileNames      = malloc(sizeof(char*) * (*list)->fileCount);
	(*list)->directoryNames = malloc(sizeof(char*) * (*list)->directoryCount);

	uint32_t fileIndex = 0, directoryIndex = 0;
	for(uint32_t index=0; index<totalCount; ++index) {
		if(isDirectory[index]) {
			(*list)->directoryNames[directoryIndex++] = strdup([[contents objectAtIndex:index] UTF8String]);
		} else {
			(*list)->fileNames[fileIndex++]           = strdup([[contents objectAtIndex:index] UTF8String]);
		}
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
