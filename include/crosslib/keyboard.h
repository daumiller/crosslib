#ifndef CROSSLIB_KEYBOARD_HEADER
#define CROSSLIB_KEYBOARD_HEADER

#include <stdint.h>
#include <stdbool.h>

//==============================================================================
// ENUMERATIONS
//==============================================================================
/// <summary>Keyboard Modifier Keys</summary>
typedef enum enum_clKeyboard_Modifier {
	// CL_KEYBOARD_INVALID = 0,
	/// <summary>Shift Key</summary>
	CL_KEYBOARD_SHIFT = 1,
	/// <summary>Ctrl Key</summary>
	CL_KEYBOARD_CONTROL = 2,
	/// <summary>Alt Key</summary>
	CL_KEYBOARD_ALTERNATE = 3,
	/// <summary>Windows/Command Key</summary>
	CL_KEYBOARD_META = 4
} clKeyboard_Modifier;

/// <summary>Keyboard Event Types</summary>
typedef enum enum_clKeyboard_Press {
	// CL_KEYBOARD_INVALID = 0,
	/// <summary>Key Pressed</summary>
	CL_KEYBOARD_PRESSED = 1,
	/// <summary>Key Released</summary>
	CL_KEYBOARD_RELEASED = 2,
} clKeyboard_Press;

/// <summary>Keyboard Key Codes</summary>
typedef enum enum_clKeyboard_Key {
	// CL_KEYBOARD_INVALID = 0,
	CL_KEYBOARD_SPACE = 1,
	CL_KEYBOARD_A, CL_KEYBOARD_B, CL_KEYBOARD_C, CL_KEYBOARD_D, CL_KEYBOARD_E,
	CL_KEYBOARD_F, CL_KEYBOARD_G, CL_KEYBOARD_H, CL_KEYBOARD_I, CL_KEYBOARD_J,
	CL_KEYBOARD_K, CL_KEYBOARD_L, CL_KEYBOARD_M, CL_KEYBOARD_N, CL_KEYBOARD_O,
	CL_KEYBOARD_P, CL_KEYBOARD_Q, CL_KEYBOARD_R, CL_KEYBOARD_S, CL_KEYBOARD_T,
	CL_KEYBOARD_U, CL_KEYBOARD_V, CL_KEYBOARD_W, CL_KEYBOARD_X, CL_KEYBOARD_Y,
	CL_KEYBOARD_Z,
	CL_KEYBOARD_0, CL_KEYBOARD_1, CL_KEYBOARD_2, CL_KEYBOARD_3, CL_KEYBOARD_4,
	CL_KEYBOARD_5, CL_KEYBOARD_6, CL_KEYBOARD_7, CL_KEYBOARD_8, CL_KEYBOARD_9,
	CL_KEYBOARD_NUM0, CL_KEYBOARD_NUM1, CL_KEYBOARD_NUM2, CL_KEYBOARD_NUM3, CL_KEYBOARD_NUM4,
	CL_KEYBOARD_NUM5, CL_KEYBOARD_NUM6, CL_KEYBOARD_NUM7, CL_KEYBOARD_NUM8, CL_KEYBOARD_NUM9,
	CL_KEYBOARD_UP, CL_KEYBOARD_DOWN, CL_KEYBOARD_LEFT, CL_KEYBOARD_RIGHT,
	CL_KEYBOARD_ESCAPE, CL_KEYBOARD_TAB, CL_KEYBOARD_BACKSPACE, CL_KEYBOARD_ENTER,
	CL_KEYBOARD_INSERT, CL_KEYBOARD_DELETE, CL_KEYBOARD_HOME, CL_KEYBOARD_END, CL_KEYBOARD_PAGEUP, CL_KEYBOARD_PAGEDOWN,
	CL_KEYBOARD_F1, CL_KEYBOARD_F2, CL_KEYBOARD_F3, CL_KEYBOARD_F4, CL_KEYBOARD_F5,
	CL_KEYBOARD_F6, CL_KEYBOARD_F7, CL_KEYBOARD_F8, CL_KEYBOARD_F9, CL_KEYBOARD_F10,
	CL_KEYBOARD_F11, CL_KEYBOARD_F12, CL_KEYBOARD_PRINTSCREEN,
	CL_KEYBOARD_VOLDOWN, CL_KEYBOARD_VOLUP, CL_KEYBOARD_VOLMUTE,
	CL_KEYBOARD_PREV, CL_KEYBOARD_NEXT, CL_KEYBOARD_PLAY,
	CL_KEYBOARD_ADD, CL_KEYBOARD_SUBTRACT, CL_KEYBOARD_MULTIPLY, CL_KEYBOARD_DIVIDE,
	CL_KEYBOARD_COMMA, CL_KEYBOARD_PERIOD
} clKeyboard_Key;

//==============================================================================
// FUNCTIONS
//==============================================================================
/// <summary>Simulate a press or release of a modifier key.</summary>
/// <param name="modifier">modifier key to simulate</param>
/// <param name="press">press or release</param>
/// <returns>boolean indicating success</returns>
bool clKeyboard_postModifier(clKeyboard_Modifier modifier, clKeyboard_Press press);

/// <summary>Simulate a press or release of a single key.</summary>
/// <param name="vkey">key to simulate</param>
/// <param name="press">press or release</param>
/// <returns>boolean indicating success</returns>
bool clKeyboard_postKey(clKeyboard_Key key, clKeyboard_Press press);

/// <summary>Simulate keyboard entry of a UTF8 string.</summary>
/// <param name="string">string to enter</param>
/// <returns>boolean indicating success</returns>
bool clKeyboard_postString(char* string);

#endif //CROSSLIB_KEYBOARD_HEADER
