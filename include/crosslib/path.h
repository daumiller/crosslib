#ifndef CROSSLIB_PATH_HEADER
#define CROSSLIB_PATH_HEADER

#include <stdint.h>
#include <stdbool.h>

//==============================================================================
// TYPES
//==============================================================================
/// <summary>Directory Listing Storage</summary>
typedef struct struct_clPath_DirectoryList {
	/// <summary>Number of files.</summary>
	uint32_t fileCount;
	/// <summary>Number of directories.</summary>
	uint32_t directoryCount;
	/// <summary>Names of files.</summary>
	char** fileNames;
	/// <summary>Names of directories.</summary>
	char** directoryNames;
} clPath_DirectoryList;

//==============================================================================
// FUNCTIONS
//==============================================================================
/// <summary>Combine path strings.</summary>
/// <param name="left">left path component</param>
/// <param name="right">right path component</param>
/// <returns>combined path, in newly allocated string</returns>
/// <remarks>Caller is repsonsible for freeing returned string. Path separator will be added as needed.</remarks>
char* clPath_combine(char* left, char* right);

/// <summary>Resolve a relative path to an absolute path.</summary>
/// <param name="path">a relative path</param>
/// <returns>absolute path, in newly allocated string</returns>
/// <remarks>Caller is responsible for freeing returned string.</remarks>
char* clPath_resolve(char* path);

/// <summary>Get current user's home directory path.</summary>
/// <returns>path to user's home directory, or NULL on error</returns>
/// <remarks>Caller is responsible for freeing returned string.</remarks>
char* clPath_getHome(void);

/// <summary>Get current working directory path.</summary>
/// <returns>path to working directory, or NULL on error</returns>
/// <remarks>Caller is responsible for freeing returned string.</remarks>
char* clPath_getWorking(void);

/// <summary>Check if path is a valid file.</summary>
/// <param name="path">path to check</param>
/// <returns>true if a file, false if not a file or on error</returns>
/// <remarks>A true return value can be trusted, but a false value may be inaccessible or an invalid path.</remarks>
bool clPath_isFile(char* path);

/// <summary>Check if path is a valid directory.</summary>
/// <param name="path">path to check</param>
/// <returns>true if a directory, false if not a directory or on error</returns>
/// <remarks>A true return value can be trusted, but a false value may be inaccessible or an invalid path.</remarks>
bool clPath_isDirectory(char* path);

/// <summary>Read an entire file's contents.</summary>
/// <param name="path">path of file to read</param>
/// <param name="data">pointer to store buffer in</param>
/// <param name="size">pointer to store file size in</param>
/// <returns>boolean indicating success</returns>
/// <remarks>Caller is responsible for freeing returned buffer.</remarks>
bool clPath_fileRead(char* path, uint8_t** data, uint32_t* size);

/// <summary>Write buffer to a file.</summary>
/// <param name="path">file path to write to</param>
/// <param name="data">data buffer to write</param>
/// <param name="size">size buffer to write</param>
/// <returns>boolean indicating success</returns>
/// <remarks>File will be created or overwrote.</remarks>
bool clPath_fileWrite(char* path, uint8_t* data, uint32_t size);

/// <summary>Get contents of directory.</summary>
/// <param name="path">path of directory to read</param>
/// <param name="list">pointer to store directory list</param>
/// <returns>boolean indicating success</returns>
/// <remarks>Caller is responsible for calling <see cref="clThread_threadReturn">clPath_directoryListFree if call succeeds.</see></remarks>
bool clPath_directoryListCreate(char* path, clPath_DirectoryList** list);

/// <summary>Free data from a created directory list.</summary>
/// <param name="list">list to clean up</param>
void clPath_directoryListFree(clPath_DirectoryList* list);

#endif //CROSSLIB_PATH_HEADER
