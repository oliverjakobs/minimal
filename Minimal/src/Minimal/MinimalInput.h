#ifndef MINIMAL_INPUT_H
#define MINIMAL_INPUT_H

#include "MinimalWindow.h"

#define MINIMAL_KEY_MOD_SHIFT       0x0001
#define MINIMAL_KEY_MOD_CONTROL     0x0002
#define MINIMAL_KEY_MOD_ALT         0x0004
#define MINIMAL_KEY_MOD_SUPER       0x0008
#define MINIMAL_KEY_MOD_CAPS_LOCK   0x0010
#define MINIMAL_KEY_MOD_NUM_LOCK    0x0020

UINT MinimalGetKeyMods();

#define MINIMAL_KEY_UNKNOWN			-1

/* Function keys */
#define MINIMAL_KEY_ESCAPE			0
#define MINIMAL_KEY_F1				1
#define MINIMAL_KEY_F2				2
#define MINIMAL_KEY_F3				3
#define MINIMAL_KEY_F4				4
#define MINIMAL_KEY_F5				5
#define MINIMAL_KEY_F6				6
#define MINIMAL_KEY_F7				7
#define MINIMAL_KEY_F8				8
#define MINIMAL_KEY_F9				9
#define MINIMAL_KEY_F10				10
#define MINIMAL_KEY_F11				11
#define MINIMAL_KEY_F12				12

#define MINIMAL_KEY_ENTER			13
#define MINIMAL_KEY_TAB				14
#define MINIMAL_KEY_BACKSPACE		15
#define MINIMAL_KEY_INSERT			16
#define MINIMAL_KEY_DELETE			17

#define MINIMAL_KEY_RIGHT			18
#define MINIMAL_KEY_LEFT			19
#define MINIMAL_KEY_DOWN			20
#define MINIMAL_KEY_UP				21

#define MINIMAL_KEY_PAGE_UP			22
#define MINIMAL_KEY_PAGE_DOWN		23
#define MINIMAL_KEY_HOME			24
#define MINIMAL_KEY_END				25
#define MINIMAL_KEY_CAPS_LOCK		26
#define MINIMAL_KEY_SCROLL_LOCK		27
#define MINIMAL_KEY_NUM_LOCK		28
#define MINIMAL_KEY_PRINT_SCREEN	29
#define MINIMAL_KEY_PAUSE			30

/* ASCII keys */
#define MINIMAL_KEY_SPACE			32
#define MINIMAL_KEY_APOSTROPHE		39	/* ' */
#define MINIMAL_KEY_COMMA			44	/* , */
#define MINIMAL_KEY_MINUS			45	/* - */
#define MINIMAL_KEY_PERIOD			46	/* . */
#define MINIMAL_KEY_SLASH			47	/* / */
#define MINIMAL_KEY_0				48
#define MINIMAL_KEY_1				49
#define MINIMAL_KEY_2				50
#define MINIMAL_KEY_3				51
#define MINIMAL_KEY_4				52
#define MINIMAL_KEY_5				53
#define MINIMAL_KEY_6				54
#define MINIMAL_KEY_7				55
#define MINIMAL_KEY_8				56
#define MINIMAL_KEY_9				57
#define MINIMAL_KEY_SEMICOLON		59	/* ; */
#define MINIMAL_KEY_EQUAL			61	/* = */
#define MINIMAL_KEY_A				65
#define MINIMAL_KEY_B				66
#define MINIMAL_KEY_C				67
#define MINIMAL_KEY_D				68
#define MINIMAL_KEY_E				69
#define MINIMAL_KEY_F				70
#define MINIMAL_KEY_G				71
#define MINIMAL_KEY_H				72
#define MINIMAL_KEY_I				73
#define MINIMAL_KEY_J				74
#define MINIMAL_KEY_K				75
#define MINIMAL_KEY_L				76
#define MINIMAL_KEY_M				77
#define MINIMAL_KEY_N				78
#define MINIMAL_KEY_O				79
#define MINIMAL_KEY_P				80
#define MINIMAL_KEY_Q				81
#define MINIMAL_KEY_R				82
#define MINIMAL_KEY_S				83
#define MINIMAL_KEY_T				84
#define MINIMAL_KEY_U				85
#define MINIMAL_KEY_V				86
#define MINIMAL_KEY_W				87
#define MINIMAL_KEY_X				88
#define MINIMAL_KEY_Y				89
#define MINIMAL_KEY_Z				90
#define MINIMAL_KEY_LEFT_BRACKET	91	/* [ */
#define MINIMAL_KEY_BACKSLASH		92	/* \ */
#define MINIMAL_KEY_RIGHT_BRACKET	93	/* ] */
#define MINIMAL_KEY_GRAVE_ACCENT	96	/* ` */

/* numpad keys */
#define MINIMAL_KEY_NP_0			256
#define MINIMAL_KEY_NP_1			257
#define MINIMAL_KEY_NP_2			258
#define MINIMAL_KEY_NP_3			259
#define MINIMAL_KEY_NP_4			260
#define MINIMAL_KEY_NP_5			261
#define MINIMAL_KEY_NP_6			262
#define MINIMAL_KEY_NP_7			263
#define MINIMAL_KEY_NP_8			264
#define MINIMAL_KEY_NP_9			265
#define MINIMAL_KEY_NP_DECIMAL		266
#define MINIMAL_KEY_NP_DIVIDE		267
#define MINIMAL_KEY_NP_MULTIPLY		268
#define MINIMAL_KEY_NP_SUBTRACT		269
#define MINIMAL_KEY_NP_ADD			270
#define MINIMAL_KEY_NP_ENTER		271
#define MINIMAL_KEY_NP_EQUAL		272

/* key mods */
#define MINIMAL_KEY_LEFT_SHIFT		273
#define MINIMAL_KEY_LEFT_CONTROL	274
#define MINIMAL_KEY_LEFT_ALT		275
#define MINIMAL_KEY_LEFT_SUPER		276
#define MINIMAL_KEY_RIGHT_SHIFT		277
#define MINIMAL_KEY_RIGHT_CONTROL	278
#define MINIMAL_KEY_RIGHT_ALT		279
#define MINIMAL_KEY_RIGHT_SUPER		280

#define MINIMAL_KEY_FIRST			MINIMAL_KEY_ESCAPE
#define MINIMAL_KEY_LAST			MINIMAL_KEY_RIGHT_SUPER

void MinimalCreateKeyTable();

/* tanslate win32 virtual key codes to minimal key codes */
int MinimalTranslateKey(UINT scancode);

typedef enum
{
    MINIMAL_RELEASE = 0,
    MINIMAL_PRESS = 1,
    MINIMAL_REPEAT = 2
} MinimalKeyState;

void MinimalUpdateKey(MinimalWindow* window, UINT key, UINT scancode, MinimalKeyState action, UINT mods);

int MinimalKeyPressed(MinimalWindow* window, UINT keycode);
int MinimalKeyReleased(MinimalWindow* window, UINT keycode);

#endif // !MINIMAL_INPUT_H
