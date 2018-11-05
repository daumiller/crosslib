#include <Carbon/Carbon.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/hidsystem/IOHIDShared.h>
#include "crosslib/keyboard.h"
#include "crosslib/string.h"

static uint16_t translateKeyCode(clKeyboard_Key key) {
	// https://stackoverflow.com/questions/3202629/where-can-i-find-a-list-of-mac-virtual-key-codes
	static uint16_t translation[] = {
		0 /* CL_KEYBOARD_INVALID */,
		kVK_Space /* CL_KEYBOARD_SPACE */,
		kVK_ANSI_A /* CL_KEYBOARD_A */, kVK_ANSI_B /* CL_KEYBOARD_B */, kVK_ANSI_C /* CL_KEYBOARD_C */, kVK_ANSI_D /* CL_KEYBOARD_D */, kVK_ANSI_E /* CL_KEYBOARD_E */,
		kVK_ANSI_F /* CL_KEYBOARD_F */, kVK_ANSI_G /* CL_KEYBOARD_G */, kVK_ANSI_H /* CL_KEYBOARD_H */, kVK_ANSI_I /* CL_KEYBOARD_I */, kVK_ANSI_J /* CL_KEYBOARD_J */,
		kVK_ANSI_K /* CL_KEYBOARD_K */, kVK_ANSI_L /* CL_KEYBOARD_L */, kVK_ANSI_M /* CL_KEYBOARD_M */, kVK_ANSI_N /* CL_KEYBOARD_N */, kVK_ANSI_O /* CL_KEYBOARD_O */,
		kVK_ANSI_P /* CL_KEYBOARD_P */, kVK_ANSI_Q /* CL_KEYBOARD_Q */, kVK_ANSI_R /* CL_KEYBOARD_R */, kVK_ANSI_S /* CL_KEYBOARD_S */, kVK_ANSI_T /* CL_KEYBOARD_T */,
		kVK_ANSI_U /* CL_KEYBOARD_U */, kVK_ANSI_V /* CL_KEYBOARD_V */, kVK_ANSI_W /* CL_KEYBOARD_W */, kVK_ANSI_X /* CL_KEYBOARD_X */, kVK_ANSI_Y /* CL_KEYBOARD_Y */,
		kVK_ANSI_Z /* CL_KEYBOARD_Z */,
		kVK_ANSI_0 /* CL_KEYBOARD_0 */, kVK_ANSI_1 /* CL_KEYBOARD_1 */, kVK_ANSI_2 /* CL_KEYBOARD_2 */, kVK_ANSI_3 /* CL_KEYBOARD_3 */, kVK_ANSI_4 /* CL_KEYBOARD_4 */,
		kVK_ANSI_5 /* CL_KEYBOARD_5 */, kVK_ANSI_6 /* CL_KEYBOARD_6 */, kVK_ANSI_7 /* CL_KEYBOARD_7 */, kVK_ANSI_8 /* CL_KEYBOARD_8 */, kVK_ANSI_9 /* CL_KEYBOARD_9 */,
		kVK_ANSI_Keypad0 /* CL_KEYBOARD_NUM0 */, kVK_ANSI_Keypad1 /* CL_KEYBOARD_NUM1 */, kVK_ANSI_Keypad2 /* CL_KEYBOARD_NUM2 */, kVK_ANSI_Keypad3 /* CL_KEYBOARD_NUM3 */, kVK_ANSI_Keypad4 /* CL_KEYBOARD_NUM4 */,
		kVK_ANSI_Keypad5 /* CL_KEYBOARD_NUM5 */, kVK_ANSI_Keypad6 /* CL_KEYBOARD_NUM6 */, kVK_ANSI_Keypad7 /* CL_KEYBOARD_NUM7 */, kVK_ANSI_Keypad8 /* CL_KEYBOARD_NUM8 */, kVK_ANSI_Keypad9 /* CL_KEYBOARD_NUM9 */,
		kVK_UpArrow /* CL_KEYBOARD_UP */, kVK_DownArrow /* CL_KEYBOARD_DOWN */, kVK_LeftArrow /* CL_KEYBOARD_LEFT */, kVK_RightArrow /* CL_KEYBOARD_RIGHT */,
		kVK_Escape /* CL_KEYBOARD_ESCAPE */, kVK_Tab /* CL_KEYBOARD_TAB */, kVK_Delete /* CL_KEYBOARD_BACKSPACE */, kVK_Return /* CL_KEYBOARD_ENTER */,
		0 /* CL_KEYBOARD_INSERT */, kVK_ForwardDelete /* CL_KEYBOARD_DELETE */,
		kVK_Home /* CL_KEYBOARD_HOME */, kVK_End /* CL_KEYBOARD_END */,
		kVK_PageUp /* CL_KEYBOARD_PAGEUP */, kVK_PageDown /* CL_KEYBOARD_PAGEDOWN */,
		kVK_F1 /* CL_KEYBOARD_F1 */, kVK_F2 /* CL_KEYBOARD_F2 */, kVK_F3 /* CL_KEYBOARD_F3 */, kVK_F4 /* CL_KEYBOARD_F4 */,
		kVK_F5 /* CL_KEYBOARD_F5 */, kVK_F6 /* CL_KEYBOARD_F6 */, kVK_F7 /* CL_KEYBOARD_F7 */, kVK_F8 /* CL_KEYBOARD_F8 */,
		kVK_F9 /* CL_KEYBOARD_F9 */, kVK_F10 /* CL_KEYBOARD_F10 */, kVK_F11 /* CL_KEYBOARD_F11 */, kVK_F12 /* CL_KEYBOARD_F12 */,
		0 /* CL_KEYBOARD_PRINTSCREEN */,
		kVK_VolumeDown /* CL_KEYBOARD_VOLDOWN */, kVK_VolumeUp/* CL_KEYBOARD_VOLUP */, kVK_Mute/* CL_KEYBOARD_VOLMUTE */,
		0 /* CL_KEYBOARD_PREV */, 0 /* CL_KEYBOARD_NEXT */, 0/* CL_KEYBOARD_PLAY */,
		kVK_ANSI_KeypadPlus /* CL_KEYBOARD_ADD */, kVK_ANSI_KeypadMinus /* CL_KEYBOARD_SUBTRACT */,
		kVK_ANSI_KeypadMultiply /* CL_KEYBOARD_MULTIPLY */, kVK_ANSI_KeypadDivide /* CL_KEYBOARD_DIVIDE */,
		kVK_ANSI_Comma /* CL_KEYBOARD_COMMA */, kVK_ANSI_KeypadDecimal /* CL_KEYBOARD_PERIOD */
	};

	if(key < CL_KEYBOARD_SPACE ) { return 0; }
	if(key > CL_KEYBOARD_PERIOD) { return 0; }

	return translation[key];
}

static uint32_t globalModifiers = 0;
static mach_port_t globalKeyboardPort = 0;

static bool getKeyboardPort(void) {
	return false;
}

bool clKeyboard_postModifier(clKeyboard_Modifier modifier, clKeyboard_Press press) {
	static uint32_t macosModifiers[5] = {
		0,                // INVALID               = 0
		NX_SHIFTMASK,     // CL_KEYBOARD_SHIFT     = 1
		NX_CONTROLMASK,   // CL_KEYBOARD_CONTROL   = 2
		NX_ALTERNATEMASK, // CL_KEYBOARD_ALTERNATE = 3
		NX_COMMANDMASK    // CL_KEYBOARD_META      = 4
	};

	if(!modifier) { return false; }
	if(!press   ) { return false; }
	if(!globalKeyboardPort && !getKeyboardPort()) { return false; }

	// pretend we can send individual key events
	if(press == CL_KEYBOARD_PRESSED) {
		globalModifiers |= macosModifiers[modifier];
	} else {
		globalModifiers &= ~macosModifiers[modifier];
	}

	NXEventData event;
	bzero(&event, sizeof(NXEventData));
	IOGPoint loc = {0, 0};
	kern_return_t kr = IOHIDPostEvent(globalKeyboardPort, NX_FLAGSCHANGED, loc, &event, kNXEventDataVersion, globalModifiers, true);
	return (kr == KERN_SUCCESS);
}

bool clKeyboard_postKey(clKeyboard_Key key, clKeyboard_Press press) {
	if(!key  ) { return false; }
	if(!press) { return false; }
	if(!globalKeyboardPort && !getKeyboardPort()) { return false; }

	uint16_t translatedKey = translateKeyCode(key);
	if(translatedKey == 0) { return false; }

	uint32_t eventType = (press == CL_KEYBOARD_PRESSED) ? NX_KEYDOWN : NX_KEYUP;

	NXEventData event;
	memset(&event, 0, sizeof(event));
	event.key.repeat = false;
	event.key.keyCode = translatedKey;
	event.key.origCharSet = event.key.charSet = NX_ASCIISET;
	event.key.origCharCode = event.key.charCode = 0;

	IOGPoint loc = {0, 0};
	kern_return_t kr = IOHIDPostEvent(globalKeyboardPort, eventType, loc, &event, kNXEventDataVersion, 0, false);
	return (kr == KERN_SUCCESS);
}

static int u16len(char16_t* string) {
    int length = 0;
    while(string[length]) { ++length; }
    return length;
}

bool clKeyboard_postString(char* string) {
	char16_t* utf16 = clString_utf16from8(string); if(!utf16) { return false; }
	
	CGEventRef keyEvent = CGEventCreateKeyboardEvent(NULL, 0, true);
	if(!keyEvent) { return false; }

	size_t utf16Len = u16len(utf16);
	for(size_t index=0; index<utf16Len; ++index) {
		CGEventKeyboardSetUnicodeString(keyEvent, 1, utf16+index);
		CGEventPost(kCGSessionEventTap, keyEvent);
	}

	CFRelease(keyEvent);
	free(utf16);

	return true;
}
