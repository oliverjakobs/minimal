#ifndef MINIMAL_H
#define MINIMAL_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


/* minimal version numbers */
#define MINIMAL_VERSION_MAJOR       1
#define MINIMAL_VERSION_MINOR       0
#define MINIMAL_VERSION_REVISION    0

void minimalGetVersion(int* major, int* minor, int* rev);
const char* minimalGetVersionString();

#define MINIMAL_FAIL    0
#define MINIMAL_OK      1

#ifndef _DEBUG
#define MINIMAL_DISABLE_LOGGING
#define MINIMAL_DISABLE_ASSERT
#endif

typedef struct MinimalWindow MinimalWindow;
typedef struct MinimalEvent MinimalEvent;

/* --------------------------| logging |--------------------------------- */
#ifndef MINIMAL_DISABLE_LOGGING

#define MINIMAL_TRACE(s, ...)       minimalLoggerPrint(MINIMAL_LOG_TRACE, s, __VA_ARGS__)
#define MINIMAL_INFO(s, ...)        minimalLoggerPrint(MINIMAL_LOG_INFO, s, __VA_ARGS__)
#define MINIMAL_WARN(s, ...)        minimalLoggerPrint(MINIMAL_LOG_WARN, s, __VA_ARGS__)
#define MINIMAL_ERROR(s, ...)       minimalLoggerPrint(MINIMAL_LOG_ERROR, s, __VA_ARGS__)
#define MINIMAL_CRITICAL(s, ...)    minimalLoggerPrint(MINIMAL_LOG_CRITICAL, s, __VA_ARGS__)

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

void minimalLoggerPrint(MinimalLogLevel level, const char* fmt, ...);
void minimalLoggerPrintV(MinimalLogLevel level, const char* fmt, va_list args);

/* --------------------------| assert |---------------------------------- */
#ifndef MINIMAL_DISABLE_ASSERT
#include <assert.h>
#define MINIMAL_ASSERT(expr, msg) assert(((void)(msg), (expr)))
#else
#define MINIMAL_ASSERT(expr, msg)
#endif

/* --------------------------| memory |---------------------------------- */
#define MINIMAL_ALLOC(size)             malloc(size)
#define MINIMAL_FREE(block, size)       free(block)
#define MINIMAL_MEMCPY(dst, src, size)  memcpy(dst, src, size);


/* --------------------------| input |----------------------------------- */
#define MINIMAL_KEY_UNKNOWN     -1

#define MINIMAL_KEY_BACKSPACE   0x08
#define MINIMAL_KEY_TAB         0x09
#define MINIMAL_KEY_ENTER       0x0D
#define MINIMAL_KEY_SHIFT       0x10
#define MINIMAL_KEY_CONTROL     0x11
#define MINIMAL_KEY_ALT         0x11
#define MINIMAL_KEY_PAUSE       0x13
#define MINIMAL_KEY_CAPITAL     0x14

#define MINIMAL_KEY_ESCAPE      0x1B

#define MINIMAL_KEY_CONVERT     0x1C
#define MINIMAL_KEY_NONCONVERT  0x1D
#define MINIMAL_KEY_ACCEPT      0x1E
#define MINIMAL_KEY_MODECHANGE  0x1F

#define MINIMAL_KEY_SPACE       0x20
#define MINIMAL_KEY_PAGE_UP     0x21
#define MINIMAL_KEY_PAGE_DOWN   0x22
#define MINIMAL_KEY_END         0x23
#define MINIMAL_KEY_HOME        0x24
#define MINIMAL_KEY_LEFT        0x25
#define MINIMAL_KEY_UP          0x26
#define MINIMAL_KEY_RIGHT       0x27
#define MINIMAL_KEY_DOWN        0x28
#define MINIMAL_KEY_SELECT      0x29
#define MINIMAL_KEY_PRINT       0x2A
#define MINIMAL_KEY_EXECUTE     0x2B
#define MINIMAL_KEY_SNAPSHOT    0x2C
#define MINIMAL_KEY_INSERT      0x2D
#define MINIMAL_KEY_DELETE      0x2E
#define MINIMAL_KEY_HELP        0x2F

#define MINIMAL_KEY_0   0x30
#define MINIMAL_KEY_1   0x31
#define MINIMAL_KEY_2   0x32
#define MINIMAL_KEY_3   0x33
#define MINIMAL_KEY_4   0x34
#define MINIMAL_KEY_5   0x35
#define MINIMAL_KEY_6   0x36
#define MINIMAL_KEY_7   0x37
#define MINIMAL_KEY_8   0x38
#define MINIMAL_KEY_9   0x39

#define MINIMAL_KEY_A   0x41
#define MINIMAL_KEY_B   0x42
#define MINIMAL_KEY_C   0x43
#define MINIMAL_KEY_D   0x44
#define MINIMAL_KEY_E   0x45
#define MINIMAL_KEY_F   0x46
#define MINIMAL_KEY_G   0x47
#define MINIMAL_KEY_H   0x48
#define MINIMAL_KEY_I   0x49
#define MINIMAL_KEY_J   0x4A
#define MINIMAL_KEY_K   0x4B
#define MINIMAL_KEY_L   0x4C
#define MINIMAL_KEY_M   0x4D
#define MINIMAL_KEY_N   0x4E
#define MINIMAL_KEY_O   0x4F
#define MINIMAL_KEY_P   0x50
#define MINIMAL_KEY_Q   0x51
#define MINIMAL_KEY_R   0x52
#define MINIMAL_KEY_S   0x53
#define MINIMAL_KEY_T   0x54
#define MINIMAL_KEY_U   0x55
#define MINIMAL_KEY_V   0x56
#define MINIMAL_KEY_W   0x57
#define MINIMAL_KEY_X   0x58
#define MINIMAL_KEY_Y   0x59
#define MINIMAL_KEY_Z   0x5A

#define MINIMAL_KEY_LCOMMAND 0x5B
#define MINIMAL_KEY_RCOMMAND 0x5C
#define MINIMAL_KEY_APPS     0x5D

#define MINIMAL_KEY_SLEEP    0x5F

#define MINIMAL_KEY_NUMPAD0     0x60
#define MINIMAL_KEY_NUMPAD1     0x61
#define MINIMAL_KEY_NUMPAD2     0x62
#define MINIMAL_KEY_NUMPAD3     0x63
#define MINIMAL_KEY_NUMPAD4     0x64
#define MINIMAL_KEY_NUMPAD5     0x65
#define MINIMAL_KEY_NUMPAD6     0x66
#define MINIMAL_KEY_NUMPAD7     0x67
#define MINIMAL_KEY_NUMPAD8     0x68
#define MINIMAL_KEY_NUMPAD9     0x69
#define MINIMAL_KEY_MULTIPLY    0x6A
#define MINIMAL_KEY_ADD         0x6B
#define MINIMAL_KEY_SEPARATOR   0x6C
#define MINIMAL_KEY_SUBTRACT    0x6D
#define MINIMAL_KEY_DECIMAL     0x6E
#define MINIMAL_KEY_DIVIDE      0x6F

#define MINIMAL_KEY_F1      0x70
#define MINIMAL_KEY_F2      0x71
#define MINIMAL_KEY_F3      0x72
#define MINIMAL_KEY_F4      0x73
#define MINIMAL_KEY_F5      0x74
#define MINIMAL_KEY_F6      0x75
#define MINIMAL_KEY_F7      0x76
#define MINIMAL_KEY_F8      0x77
#define MINIMAL_KEY_F9      0x78
#define MINIMAL_KEY_F10     0x79
#define MINIMAL_KEY_F11     0x7A
#define MINIMAL_KEY_F12     0x7B
#define MINIMAL_KEY_F13     0x7C
#define MINIMAL_KEY_F14     0x7D
#define MINIMAL_KEY_F15     0x7E
#define MINIMAL_KEY_F16     0x7F
#define MINIMAL_KEY_F17     0x80
#define MINIMAL_KEY_F18     0x81
#define MINIMAL_KEY_F19     0x82
#define MINIMAL_KEY_F20     0x83
#define MINIMAL_KEY_F21     0x84
#define MINIMAL_KEY_F22     0x85
#define MINIMAL_KEY_F23     0x86
#define MINIMAL_KEY_F24     0x87

#define MINIMAL_KEY_NUMLOCK 0x90
#define MINIMAL_KEY_SCROLL  0x91

#define MINIMAL_KEY_NUMPAD_EQUAL     0x92

#define MINIMAL_KEY_LSHIFT      0xA0
#define MINIMAL_KEY_RSHIFT      0xA1
#define MINIMAL_KEY_LCONTROL    0xA2
#define MINIMAL_KEY_RCONTROL    0xA3
#define MINIMAL_KEY_LMENU       0xA4
#define MINIMAL_KEY_RMENU       0xA5

#define MINIMAL_KEY_SEMICOLON   0xBA
#define MINIMAL_KEY_PLUS        0xBB
#define MINIMAL_KEY_COMMA       0xBC
#define MINIMAL_KEY_MINUS       0xBD
#define MINIMAL_KEY_PERIOD      0xBE
#define MINIMAL_KEY_SLASH       0xBF
#define MINIMAL_KEY_GRAVE       0xC0

#define MINIMAL_KEY_FIRST       MINIMAL_KEY_BACKSPACE
#define MINIMAL_KEY_LAST        MINIMAL_KEY_GRAVE

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
#define MINIMAL_KEY_MOD_COMMAND     0x0008
#define MINIMAL_KEY_MOD_CAPS_LOCK   0x0010
#define MINIMAL_KEY_MOD_NUM_LOCK    0x0020

/* input action */
#define MINIMAL_PRESS   1
#define MINIMAL_RELEASE 0

typedef int16_t MinimalKeycode;
typedef int8_t  MinimalMouseButton;

void minimalUpdateInput();

uint8_t minimalProcessKey(MinimalKeycode keycode, uint8_t action);
uint8_t minimalProcessMouseButton(MinimalMouseButton button, uint8_t action);
uint8_t minimalProcessMouseMove(float x, float y);

uint8_t minimalKeycodeValid(MinimalKeycode keycode);
uint8_t minimalMouseButtonValid(MinimalMouseButton button);

uint8_t minimalKeyPressed(MinimalKeycode keycode);
uint8_t minimalKeyReleased(MinimalKeycode keycode);
uint8_t minimalKeyDown(MinimalKeycode keycode);

uint8_t minimalKeyModActive(uint32_t keymod);

uint8_t minimalMousePressed(MinimalMouseButton button);
uint8_t minimalMouseReleased(MinimalMouseButton button);
uint8_t minimalMouseDown(MinimalMouseButton button);

void minimalCursorPos(float* x, float* y);
float minimalCursorX();
float minimalCursorY();

/* --------------------------| event |----------------------------------- */
#define MINIMAL_EVENT_UNKOWN            0

/* window events*/
#define MINIMAL_EVENT_WINDOW_SIZE       1
#define MINIMAL_EVENT_WINDOW_MINIMIZE   2
#define MINIMAL_EVENT_WINDOW_MAXIMIZE   3

/* key events */
#define MINIMAL_EVENT_KEY               10
#define MINIMAL_EVENT_CHAR              11

/* mouse events */
#define MINIMAL_EVENT_MOUSE_BUTTON      12
#define MINIMAL_EVENT_MOUSE_MOVED       13
#define MINIMAL_EVENT_MOUSE_SCROLLED    14

#define MINIMAL_EVENT_LAST              MINIMAL_EVENT_MOUSE_SCROLLED

/* Dispatch */
typedef uint8_t(*MinimalEventCB)(void* context, const MinimalEvent* e);
void minimalSetEventHandler(void* context, MinimalEventCB callback);

void minimalDispatchEvent(uint32_t type, uint32_t uParam, int32_t lParam, int32_t rParam);
void minimalDispatchExternalEvent(uint32_t type, const void* data);

/* Utility */
uint8_t minimalEventIsType(const MinimalEvent* e, uint32_t type);
uint8_t minimalEventIsExternal(const MinimalEvent* e);

const void* minimalExternalEvent(const MinimalEvent* e);

uint8_t minimalEventWindowSize(const MinimalEvent* e, uint32_t* w, uint32_t* h);

uint8_t minimalEventMouseButton(const MinimalEvent* e, MinimalMouseButton button, float* x, float* y);
uint8_t minimalEventMouseButtonPressed(const MinimalEvent* e, MinimalMouseButton button, float* x, float* y);
uint8_t minimalEventMouseButtonReleased(const MinimalEvent* e, MinimalMouseButton button, float* x, float* y);
uint16_t minimalEventMouseButtonAction(const MinimalEvent* e);

uint8_t minimalEventMouseMoved(const MinimalEvent* e, float* x, float* y);
uint8_t minimalEventMouseScrolled(const MinimalEvent* e, float* xoffset, float* yoffset);

MinimalKeycode minimalEventKey(const MinimalEvent* e);
MinimalKeycode minimalEventKeyPressed(const MinimalEvent* e);
MinimalKeycode minimalEventKeyReleased(const MinimalEvent* e);

char minimalEventChar(const MinimalEvent* e);


/* --------------------------| platform |-------------------------------- */
/*
 Possible platforms

#define MINIMAL_PLATFORM_WINDOWS
#define MINIMAL_PLATFORM_GLFW

dissable opengl context creation with:
#define MINIMAL_NO_CONTEXT

*/

typedef enum
{
    MINIMAL_HINT_CONTEXT_MAJOR_VERSION,
    MINIMAL_HINT_CONTEXT_MINOR_VERSION,
    MINIMAL_HINT_MAX_ENUM
} MinimalWindowHint;

void minimalSetWindowHint(MinimalWindowHint name, int32_t value);

uint8_t minimalPlatformInit();
uint8_t minimalPlatformTerminate();

MinimalWindow* minimalCreateWindow(const char* title, int32_t x, int32_t y, uint32_t w, uint32_t h);
void minimalDestroyWindow(MinimalWindow* window);

void minimalSetWindowTitle(MinimalWindow* window, const char* title);

void minimalPollWindowEvents(MinimalWindow* window);

uint8_t minimalShouldClose(const MinimalWindow* window);
void minimalClose(MinimalWindow* window);

void minimalMaximize(MinimalWindow* window);
void minimalMinimize(MinimalWindow* window);

double minimalGetTime();

void minimalGetFramebufferSize(const MinimalWindow* context, int32_t* w, int32_t* h);
void minimalGetWindowContentScale(const MinimalWindow* context, float* xscale, float* yscale);

void* minimalGetNativeWindowHandle(const MinimalWindow* window);

#ifndef MINIMAL_NO_CONTEXT

void* minimalGetGLProcAddress(const char* name);
void minimalSwapInterval(uint8_t interval);

void minimalSwapBuffers(MinimalWindow* context);

#endif

/* --------------------------| game loop |------------------------------- */
typedef struct
{
    float deltatime;
    uint32_t fps;
} MinimalFrameData;

typedef void (*MinimalTickCB)(void* context, const MinimalFrameData*);
void minimalRun(MinimalWindow* window, MinimalTickCB on_tick, void* context);

void minimalClose(MinimalWindow* window);

/* --------------------------| context |--------------------------------- */
void minimalSetCurrentContext(MinimalWindow* context);
MinimalWindow* minimalGetCurrentContext();

#endif // !MINIMAL_H
