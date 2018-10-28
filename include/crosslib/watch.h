#ifndef CROSSLIB_WATCH_HEADER
#define CROSSLIB_WATCH_HEADER

#include <stdint.h>
#include <stdbool.h>

//==============================================================================
// TYPES
//==============================================================================
/// <summary>Watched directory details container.</summary>
typedef struct struct_clWatch_ChangeHook {
	/// <summary>Path being watched.</summary>
	char* path;
	/// <summary>User data.</summary>
	void* userData;
	/// <summary>Platform specific data (do not alter).</summary>
	void* platformData;
} clWatch_ChangeHook;

/// <summary>Directory modified callback function.</summary>
typedef void (*clWatch_ChangeFunction)(clWatch_ChangeHook* changeHook);

//==============================================================================
// FUNCTIONS
//==============================================================================
/// <summary>Create a hook to watch a directory, and its files, for changes.</summary>
/// <param name="path">path of directory to watch</param>
/// <param name="callback">function to be called when changes occur</param>
/// <param name="userData">any data needing passed to the callback function</param>
/// <param name="hook">location to store newly allocated change hook structure in</param>
/// <returns>boolean indicating success</returns>
/// <remarks>Caller is responsible for freeing created structure on success. Individual files cannot be watched; parent directory will notify when any contents are changed. Each created hook spawns a new thread.</remarks>
bool clWatch_changeHookCreate(char* path, clWatch_ChangeFunction callback, void* userData, clWatch_ChangeHook** hook);

/// <summary>Free data in a change hook container.</summary>
/// <param name="hook">change hook to free</param>
/// <remarks>Function will block while waiting for the hook thread to terminate.</remarks>
void clWatch_changeHookFree(clWatch_ChangeHook* hook);

#endif //CROSSLIB_WATCH_HEADER
