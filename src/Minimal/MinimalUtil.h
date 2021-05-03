#ifndef MINIMAL_UTIL_H
#define MINIMAL_UTIL_H

#define MINIMAL_FAIL    0
#define MINIMAL_OK      1

#ifndef _DEBUG
#define MINIMAL_DISABLE_LOGGING
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>

typedef struct MinimalWindow MinimalWindow;


#define MINIMAL_FALSE   0
#define MINIMAL_TRUE    1
typedef uint8_t MinimalBool;

/* --------------------------| input |----------------------------------- */
#define MINIMAL_KEY_UNKNOWN			-1

 /* function keys */
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
#define MINIMAL_KEY_RIGHT_SUPER     280

#define MINIMAL_KEY_FIRST			MINIMAL_KEY_ESCAPE
#define MINIMAL_KEY_LAST			MINIMAL_KEY_RIGHT_SUPER

/* mouse buttons */
#define MINIMAL_MOUSE_BUTTON_UNKNOWN	-1

#define MINIMAL_MOUSE_BUTTON_1			0
#define MINIMAL_MOUSE_BUTTON_2			1
#define MINIMAL_MOUSE_BUTTON_3			2
#define MINIMAL_MOUSE_BUTTON_4			3
#define MINIMAL_MOUSE_BUTTON_5			4

#define MINIMAL_MOUSE_BUTTON_LAST		MINIMAL_MOUSE_BUTTON_5

#define MINIMAL_MOUSE_BUTTON_LEFT		MINIMAL_MOUSE_BUTTON_1
#define MINIMAL_MOUSE_BUTTON_RIGHT		MINIMAL_MOUSE_BUTTON_2
#define MINIMAL_MOUSE_BUTTON_MIDDLE		MINIMAL_MOUSE_BUTTON_3

/* key mods */
#define MINIMAL_KEY_MOD_SHIFT       0x0001
#define MINIMAL_KEY_MOD_CONTROL     0x0002
#define MINIMAL_KEY_MOD_ALT         0x0004
#define MINIMAL_KEY_MOD_SUPER       0x0008
#define MINIMAL_KEY_MOD_CAPS_LOCK   0x0010
#define MINIMAL_KEY_MOD_NUM_LOCK    0x0020

/* input action */
#define MINIMAL_RELEASE 0
#define MINIMAL_PRESS   1

typedef struct
{
    uint8_t current;
    uint8_t previous;

    uint8_t action;
} MinimalInputState;

MinimalBool MinimalKeycodeValid(uint32_t keycode);
MinimalBool MinimalMouseButtonValid(uint32_t buttoncode);

/* --------------------------| logging |--------------------------------- */
#ifndef MINIMAL_DISABLE_LOGGING

#define MINIMAL_TRACE(s, ...)		MinimalLoggerPrint(stdout, MINIMAL_LOG_TRACE, s, __VA_ARGS__)
#define MINIMAL_INFO(s, ...)		MinimalLoggerPrint(stdout, MINIMAL_LOG_INFO, s, __VA_ARGS__)
#define MINIMAL_WARN(s, ...)		MinimalLoggerPrint(stdout, MINIMAL_LOG_WARN, s, __VA_ARGS__)
#define MINIMAL_ERROR(s, ...)		MinimalLoggerPrint(stdout, MINIMAL_LOG_ERROR, s, __VA_ARGS__)
#define MINIMAL_CRITICAL(s, ...)	MinimalLoggerPrint(stdout, MINIMAL_LOG_CRITICAL, s, __VA_ARGS__)

#else

#define MINIMAL_TRACE(s, ...)
#define MINIMAL_INFO(s, ...)
#define MINIMAL_WARN(s, ...)
#define MINIMAL_ERROR(s, ...)
#define MINIMAL_CRITICAL(s, ...)

#endif

typedef enum
{
    MINIMAL_LOG_TRACE,
    MINIMAL_LOG_INFO,
    MINIMAL_LOG_WARN,
    MINIMAL_LOG_ERROR,
    MINIMAL_LOG_CRITICAL
} MinimalLogLevel;

void MinimalLoggerPrint(FILE* const stream, MinimalLogLevel level, const char* fmt, ...);
void MinimalLoggerPrintV(FILE* const stream, MinimalLogLevel level, const char* fmt, va_list args);

/* --------------------------| error callback |-------------------------- */
typedef void(*MinimalErrorCB)(MinimalLogLevel level, const char* modul, const char* fmt, va_list args);

void MinimalSetErrorCallback(MinimalErrorCB callback);
void MinimalErrorCallback(MinimalLogLevel level, const char* modul, const char* fmt, ...);

/* --------------------------| timer |----------------------------------- */
typedef struct
{
    uint32_t frames;
    uint32_t fps;

    double seconds;
    double deltatime;
    double lastframe;
} MinimalTimer;

void MinimalTimerReset(MinimalTimer* timer);

void MinimalTimerStart(MinimalTimer* timer, double seconds);
void MinimalTimerEnd(MinimalTimer* timer, double seconds);

/* --------------------------| memory |---------------------------------- */
typedef void* (*MinimalAllocCB) (void* user_data, size_t size);
typedef void  (*MinimalFreeCB)  (void* user_data, void* block);

/* To enable custom memory allocation call before MinimalLoad */
void MinimalSetAllocator(void* allocator, MinimalAllocCB alloc, MinimalFreeCB free);
void* MinimalAlloc(size_t size);
void MinimalFree(void* block);

#endif // !MINIMAL_UTIL_H
