#include "crosslib/keyboard.h"
#include "crosslib/string.h"
#include <Windows.h>

static uint16_t translateKeyCode(clKeyboard_Key key) {
	// https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
	static uint16_t translation[] = {
		0 /* CL_KEYBOARD_INVALID */,
		VK_SPACE /* CL_KEYBOARD_SPACE */,
		0x41 /* CL_KEYBOARD_A */, 0x42 /* CL_KEYBOARD_B */, 0x43 /* CL_KEYBOARD_C */, 0x44 /* CL_KEYBOARD_D */, 0x45 /* CL_KEYBOARD_E */,
		0x46 /* CL_KEYBOARD_F */, 0x47 /* CL_KEYBOARD_G */, 0x48 /* CL_KEYBOARD_H */, 0x49 /* CL_KEYBOARD_I */, 0x4A /* CL_KEYBOARD_J */,
		0x4B /* CL_KEYBOARD_K */, 0x4C /* CL_KEYBOARD_L */, 0x4D /* CL_KEYBOARD_M */, 0x4E /* CL_KEYBOARD_N */, 0x4F /* CL_KEYBOARD_O */,
		0x50 /* CL_KEYBOARD_P */, 0x51 /* CL_KEYBOARD_Q */, 0x52 /* CL_KEYBOARD_R */, 0x53 /* CL_KEYBOARD_S */, 0x54 /* CL_KEYBOARD_T */,
		0x55 /* CL_KEYBOARD_U */, 0x56 /* CL_KEYBOARD_V */, 0x57 /* CL_KEYBOARD_W */, 0x58 /* CL_KEYBOARD_X */, 0x59 /* CL_KEYBOARD_Y */,
		0x5A /* CL_KEYBOARD_Z */,
		0x30 /* CL_KEYBOARD_0 */, 0x31 /* CL_KEYBOARD_1 */, 0x32 /* CL_KEYBOARD_2 */, 0x33 /* CL_KEYBOARD_3 */, 0x34 /* CL_KEYBOARD_4 */,
		0x35 /* CL_KEYBOARD_5 */, 0x36 /* CL_KEYBOARD_6 */, 0x37 /* CL_KEYBOARD_7 */, 0x38 /* CL_KEYBOARD_8 */, 0x39 /* CL_KEYBOARD_9 */,
		0x60 /* CL_KEYBOARD_NUM0 */, 0x61 /* CL_KEYBOARD_NUM1 */, 0x62 /* CL_KEYBOARD_NUM2 */, 0x63 /* CL_KEYBOARD_NUM3 */, 0x64 /* CL_KEYBOARD_NUM4 */,
		0x65 /* CL_KEYBOARD_NUM5 */, 0x66 /* CL_KEYBOARD_NUM6 */, 0x67 /* CL_KEYBOARD_NUM7 */, 0x68 /* CL_KEYBOARD_NUM8 */, 0x69 /* CL_KEYBOARD_NUM9 */,
		VK_UP /* CL_KEYBOARD_UP */, VK_DOWN /* CL_KEYBOARD_DOWN */, VK_LEFT /* CL_KEYBOARD_LEFT */, VK_RIGHT /* CL_KEYBOARD_RIGHT */,
		VK_ESCAPE /* CL_KEYBOARD_ESCAPE */, VK_TAB /* CL_KEYBOARD_TAB */, VK_BACK /* CL_KEYBOARD_BACKSPACE */, VK_RETURN /* CL_KEYBOARD_ENTER */,
		VK_INSERT /* CL_KEYBOARD_INSERT */, VK_DELETE /* CL_KEYBOARD_DELETE */,
		VK_HOME /* CL_KEYBOARD_HOME */, VK_END /* CL_KEYBOARD_END */,
		VK_PRIOR /* CL_KEYBOARD_PAGEUP */, VK_NEXT /* CL_KEYBOARD_PAGEDOWN */,
		VK_F1 /* CL_KEYBOARD_F1 */, VK_F2 /* CL_KEYBOARD_F2 */, VK_F3 /* CL_KEYBOARD_F3 */, VK_F4 /* CL_KEYBOARD_F4 */,
		VK_F5 /* CL_KEYBOARD_F5 */, VK_F6 /* CL_KEYBOARD_F6 */, VK_F7 /* CL_KEYBOARD_F7 */, VK_F8 /* CL_KEYBOARD_F8 */,
		VK_F9 /* CL_KEYBOARD_F9 */, VK_F10 /* CL_KEYBOARD_F10 */, VK_F11 /* CL_KEYBOARD_F11 */, VK_F12 /* CL_KEYBOARD_F12 */,
		VK_SNAPSHOT /* CL_KEYBOARD_PRINTSCREEN */,
		VK_VOLUME_DOWN /* CL_KEYBOARD_VOLDOWN */, VK_VOLUME_UP/* CL_KEYBOARD_VOLUP */, VK_VOLUME_MUTE/* CL_KEYBOARD_VOLMUTE */,
		VK_MEDIA_PREV_TRACK /* CL_KEYBOARD_PREV */, VK_MEDIA_NEXT_TRACK /* CL_KEYBOARD_NEXT */, VK_MEDIA_PLAY_PAUSE/* CL_KEYBOARD_PLAY */,
		VK_ADD /* CL_KEYBOARD_ADD */, VK_SUBTRACT /* CL_KEYBOARD_SUBTRACT */,
		VK_MULTIPLY /* CL_KEYBOARD_MULTIPLY */, VK_DIVIDE /* CL_KEYBOARD_DIVIDE */,
		VK_OEM_COMMA /* CL_KEYBOARD_COMMA */, VK_OEM_PERIOD /* CL_KEYBOARD_PERIOD */
	};

	if(key < CL_KEYBOARD_SPACE ) { return 0; }
	if(key > CL_KEYBOARD_PERIOD) { return 0; }

	return translation[key];
}

static bool postVKey(uint16_t vkey, clKeyboard_Press press) {
	INPUT input;
	input.type           = INPUT_KEYBOARD;
	input.ki.wVk         = vkey;
	input.ki.wScan       = 0;
	input.ki.dwFlags     = (press == CL_KEYBOARD_PRESSED) ? 0 : KEYEVENTF_KEYUP;
	input.ki.time        = 0;
	input.ki.dwExtraInfo = (ULONG_PTR)NULL;

	return (SendInput(1, &input, sizeof(INPUT)) == 1);
}

bool clKeyboard_postModifier(clKeyboard_Modifier modifier, clKeyboard_Press press) {
	static uint16_t windowsModifiers[5] = {
		0,          // INVALID               = 0
		VK_SHIFT,   // CL_KEYBOARD_SHIFT     = 1
		VK_CONTROL, // CL_KEYBOARD_CONTROL   = 2
		VK_MENU,    // CL_KEYBOARD_ALTERNATE = 3
		VK_LWIN     // CL_KEYBOARD_META      = 4
	};

	if(!modifier) { return false; }
	if(!press   ) { return false; }

	return postVKey(windowsModifiers[modifier], press);
}

bool clKeyboard_postKey(clKeyboard_Key key, clKeyboard_Press press) {
	if(!key  ) { return false; }
	if(!press) { return false; }

	uint16_t translatedKey = translateKeyCode(key);
	if(translatedKey == 0) { return false; }

	return postVKey(translatedKey, press);
}

bool clKeyboard_postString(char* string) {
	char16_t* utf16 = clString_utf16from8(string); if(!utf16) { return false; }
	
	size_t length = wcslen(utf16);
	INPUT* inputs = malloc(sizeof(INPUT) * length);
	for(size_t index=0; index<length; ++index) {
		memset(&(inputs[index]), 0, sizeof(INPUT));
		inputs[index].type           = INPUT_KEYBOARD;
		inputs[index].ki.wVk         = 0;
		inputs[index].ki.wScan       = utf16[index];
		inputs[index].ki.dwFlags     = KEYEVENTF_UNICODE;
		inputs[index].ki.time        = 0;
		inputs[index].ki.dwExtraInfo = (ULONG_PTR)NULL;
	}
	free(utf16);

	UINT posted = SendInput(length, inputs, sizeof(INPUT));
	free(inputs);

	return (posted == length);
}
