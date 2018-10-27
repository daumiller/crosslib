#ifndef CROSSLIB_WINDOW_HEADER
#define CROSSLIB_WINDOW_HEADER

#include <stdint.h>
#include <stdbool.h>

//==============================================================================
// TYPES
//==============================================================================
/// <summary>Active window details container.</summary>
typedef struct struct_clWindow_ActiveWindow {
	/// <summary>Active window's window-title.</summary>
	char* title;
	/// <summary>Active window's executable path.</summary>
	char* program;
	/// <summary>Active window's PID.</summary>
	uint64_t pid;
} clWindow_ActiveWindow;

/// <summary>Active window callback function.</summary>
typedef void (*clWindow_ActiveWindowCallback)(clWindow_ActiveWindow* window);

//==============================================================================
// ACTIVE WINDOW FUNCTIONS
//==============================================================================
/// <summary>Get details of the currently active window.</summary>
/// <param name="window">pointer to active window structure to store data in</param>
/// <returns>boolean indicating success</returns>
/// <remarks>ActiveWindow structure is not allocated; but data inside is, and caller responsible for calling <see cref="clWindow_activeWindowFree">clWindow_activeWindowFree</see>.</remarks>
bool clWindow_activeWindowRead(clWindow_ActiveWindow* window);

/// <summary>Free data used by and active window structure.</summary>
/// <param name="window">active window structure to clean up</param>
void clWindow_activeWindowFree(clWindow_ActiveWindow* window);

/// <summary>Hook into platform updates of active window changes.</summary>
/// <param name="callback">callback to receive active-window-changed updates</param>
/// <returns>boolean indicating success</returns>
/// <remarks>This function should only be called once. Function depends on a platform GUI loop running.</remarks>
bool clWindow_activeHookStart(clWindow_ActiveWindowCallback callback);

/// <summary>Check if platform hook for active window changes has been installed.</summary>
/// <returns>boolean indicating if hook running</returns>
bool clWindow_activeHookRunning(void);

/// <summary>Stop platform hook on active window changes.</summary>
/// <returns>boolean indicating success</returns>
bool clWindow_activeHookStop(void);

//==============================================================================
// GUI MAIN LOOP FUNCTIONS
//==============================================================================
/// <summary>Begin platform-specific startup of main GUI loop.</summary>
/// <param name="guiData">platform specific required startup data</param>
/// <returns>boolean indicating success</returns>
/// <remarks>On Windows, guiData should be a pointer to an HINSTANCE from the entry function. On MacOS, guiData should be NULL.</remarks>
bool clWindow_guiLoopBegin(void* guiData);

/// <summary>Stop the main GUI loop.</summary>
/// <remarks>On some platforms, this may terminate the program entirely (skipping any cleanup after its call).</remarks>
void clWindow_guiLoopStop(void);

#endif //CROSSLIB_WINDOW_HEADER
