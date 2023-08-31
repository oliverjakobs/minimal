#ifndef MINIMAL_COMMON_H
#define MINIMAL_COMMON_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define MINIMAL_FAIL    0
#define MINIMAL_OK      1

#ifndef _DEBUG
#define MINIMAL_DISABLE_LOGGING
#define MINIMAL_DISABLE_ASSERT
#endif

typedef struct MinimalApp MinimalApp;
typedef struct MinimalWindow MinimalWindow;
typedef struct MinimalEvent MinimalEvent;

/* --------------------------| logging |--------------------------------- */
#ifndef MINIMAL_DISABLE_LOGGING

#define MINIMAL_TRACE(s, ...)       minimalLoggerPrint(stdout, MINIMAL_LOG_TRACE, s, __VA_ARGS__)
#define MINIMAL_INFO(s, ...)        minimalLoggerPrint(stdout, MINIMAL_LOG_INFO, s, __VA_ARGS__)
#define MINIMAL_WARN(s, ...)        minimalLoggerPrint(stdout, MINIMAL_LOG_WARN, s, __VA_ARGS__)
#define MINIMAL_ERROR(s, ...)       minimalLoggerPrint(stdout, MINIMAL_LOG_ERROR, s, __VA_ARGS__)
#define MINIMAL_CRITICAL(s, ...)    minimalLoggerPrint(stdout, MINIMAL_LOG_CRITICAL, s, __VA_ARGS__)

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

void minimalLoggerPrint(FILE* const stream, MinimalLogLevel level, const char* fmt, ...);
void minimalLoggerPrintV(FILE* const stream, MinimalLogLevel level, const char* fmt, va_list args);

/* --------------------------| assert |---------------------------------- */
#ifndef MINIMAL_DISABLE_ASSERT
#include <assert.h>
#define MINIMAL_ASSERT(expr, msg) assert(((void)(msg), (expr)))
#else
#define MINIMAL_ASSERT(expr, msg) 
#endif

/* --------------------------| keycodes |-------------------------------- */
#define MINIMAL_KEY_UNKNOWN         -1

/* Printable keys */
#define MINIMAL_KEY_SPACE           32
#define MINIMAL_KEY_APOSTROPHE      39 /* ' */
#define MINIMAL_KEY_COMMA           44 /* , */
#define MINIMAL_KEY_MINUS           45 /* - */
#define MINIMAL_KEY_PERIOD          46 /* . */
#define MINIMAL_KEY_SLASH           47 /* / */
#define MINIMAL_KEY_0               48
#define MINIMAL_KEY_1               49
#define MINIMAL_KEY_2               50
#define MINIMAL_KEY_3               51
#define MINIMAL_KEY_4               52
#define MINIMAL_KEY_5               53
#define MINIMAL_KEY_6               54
#define MINIMAL_KEY_7               55
#define MINIMAL_KEY_8               56
#define MINIMAL_KEY_9               57
#define MINIMAL_KEY_SEMICOLON       59 /* ; */
#define MINIMAL_KEY_EQUAL           61 /* = */
#define MINIMAL_KEY_A               65
#define MINIMAL_KEY_B               66
#define MINIMAL_KEY_C               67
#define MINIMAL_KEY_D               68
#define MINIMAL_KEY_E               69
#define MINIMAL_KEY_F               70
#define MINIMAL_KEY_G               71
#define MINIMAL_KEY_H               72
#define MINIMAL_KEY_I               73
#define MINIMAL_KEY_J               74
#define MINIMAL_KEY_K               75
#define MINIMAL_KEY_L               76
#define MINIMAL_KEY_M               77
#define MINIMAL_KEY_N               78
#define MINIMAL_KEY_O               79
#define MINIMAL_KEY_P               80
#define MINIMAL_KEY_Q               81
#define MINIMAL_KEY_R               82
#define MINIMAL_KEY_S               83
#define MINIMAL_KEY_T               84
#define MINIMAL_KEY_U               85
#define MINIMAL_KEY_V               86
#define MINIMAL_KEY_W               87
#define MINIMAL_KEY_X               88
#define MINIMAL_KEY_Y               89
#define MINIMAL_KEY_Z               90
#define MINIMAL_KEY_LEFT_BRACKET    91 /* [ */
#define MINIMAL_KEY_BACKSLASH       92 /* \ */
#define MINIMAL_KEY_RIGHT_BRACKET   93 /* ] */
#define MINIMAL_KEY_GRAVE_ACCENT    96 /* ` */

/* function keys */
#define MINIMAL_KEY_ESCAPE          256
#define MINIMAL_KEY_ENTER           257
#define MINIMAL_KEY_TAB             258
#define MINIMAL_KEY_BACKSPACE       259
#define MINIMAL_KEY_INSERT          260
#define MINIMAL_KEY_DELETE          261
#define MINIMAL_KEY_RIGHT           262
#define MINIMAL_KEY_LEFT            263
#define MINIMAL_KEY_DOWN            264
#define MINIMAL_KEY_UP              265
#define MINIMAL_KEY_PAGE_UP         266
#define MINIMAL_KEY_PAGE_DOWN       267
#define MINIMAL_KEY_HOME            268
#define MINIMAL_KEY_END             269
#define MINIMAL_KEY_CAPS_LOCK       280
#define MINIMAL_KEY_SCROLL_LOCK     281
#define MINIMAL_KEY_NUM_LOCK        282
#define MINIMAL_KEY_PRINT_SCREEN    283
#define MINIMAL_KEY_PAUSE           284
#define MINIMAL_KEY_F1              290
#define MINIMAL_KEY_F2              291
#define MINIMAL_KEY_F3              292
#define MINIMAL_KEY_F4              293
#define MINIMAL_KEY_F5              294
#define MINIMAL_KEY_F6              295
#define MINIMAL_KEY_F7              296
#define MINIMAL_KEY_F8              297
#define MINIMAL_KEY_F9              298
#define MINIMAL_KEY_F10             299
#define MINIMAL_KEY_F11             300
#define MINIMAL_KEY_F12             301
#define GLFW_KEY_F13                302
#define GLFW_KEY_F14                303
#define GLFW_KEY_F15                304
#define GLFW_KEY_F16                305
#define GLFW_KEY_F17                306
#define GLFW_KEY_F18                307
#define GLFW_KEY_F19                308
#define GLFW_KEY_F20                309
#define GLFW_KEY_F21                310
#define GLFW_KEY_F22                311
#define GLFW_KEY_F23                312
#define GLFW_KEY_F24                313
#define GLFW_KEY_F25                314

/* numpad keys */
#define MINIMAL_KEY_NP_0            320
#define MINIMAL_KEY_NP_1            321
#define MINIMAL_KEY_NP_2            322
#define MINIMAL_KEY_NP_3            323
#define MINIMAL_KEY_NP_4            324
#define MINIMAL_KEY_NP_5            325
#define MINIMAL_KEY_NP_6            326
#define MINIMAL_KEY_NP_7            327
#define MINIMAL_KEY_NP_8            328
#define MINIMAL_KEY_NP_9            329
#define MINIMAL_KEY_NP_DECIMAL      330
#define MINIMAL_KEY_NP_DIVIDE       331
#define MINIMAL_KEY_NP_MULTIPLY     332
#define MINIMAL_KEY_NP_SUBTRACT     333
#define MINIMAL_KEY_NP_ADD          334
#define MINIMAL_KEY_NP_ENTER        335
#define MINIMAL_KEY_NP_EQUAL        336

/* key mods */
#define MINIMAL_KEY_LEFT_SHIFT      340
#define MINIMAL_KEY_LEFT_CONTROL    341
#define MINIMAL_KEY_LEFT_ALT        342
#define MINIMAL_KEY_LEFT_SUPER      343
#define MINIMAL_KEY_RIGHT_SHIFT     344
#define MINIMAL_KEY_RIGHT_CONTROL   345
#define MINIMAL_KEY_RIGHT_ALT       346
#define MINIMAL_KEY_RIGHT_SUPER     347
#define MINIMAL_KEY_MENU            348

#define MINIMAL_KEY_FIRST           MINIMAL_KEY_SPACE
#define MINIMAL_KEY_LAST            MINIMAL_KEY_MENU

/* mouse buttons */
#define MINIMAL_MOUSE_BUTTON_UNKNOWN    -1

#define MINIMAL_MOUSE_BUTTON_1          0
#define MINIMAL_MOUSE_BUTTON_2          1
#define MINIMAL_MOUSE_BUTTON_3          2
#define MINIMAL_MOUSE_BUTTON_4          3
#define MINIMAL_MOUSE_BUTTON_5          4
#define MINIMAL_MOUSE_BUTTON_6          5
#define MINIMAL_MOUSE_BUTTON_7          6
#define MINIMAL_MOUSE_BUTTON_8          7

#define MINIMAL_MOUSE_BUTTON_LAST       MINIMAL_MOUSE_BUTTON_8

#define MINIMAL_MOUSE_BUTTON_LEFT       MINIMAL_MOUSE_BUTTON_1
#define MINIMAL_MOUSE_BUTTON_RIGHT      MINIMAL_MOUSE_BUTTON_2
#define MINIMAL_MOUSE_BUTTON_MIDDLE     MINIMAL_MOUSE_BUTTON_3

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

uint8_t minimalKeycodeValid(int16_t keycode);
uint8_t minimalMouseButtonValid(int8_t button);

#endif // !MINIMAL_COMMON_H
