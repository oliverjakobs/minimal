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

#define MINIMAL_KEY_A    0x41
#define MINIMAL_KEY_B    0x42
#define MINIMAL_KEY_C    0x43
#define MINIMAL_KEY_D    0x44
#define MINIMAL_KEY_E    0x45
#define MINIMAL_KEY_F    0x46
#define MINIMAL_KEY_G    0x47
#define MINIMAL_KEY_H    0x48
#define MINIMAL_KEY_I    0x49
#define MINIMAL_KEY_J    0x4A
#define MINIMAL_KEY_K    0x4B
#define MINIMAL_KEY_L    0x4C
#define MINIMAL_KEY_M    0x4D
#define MINIMAL_KEY_N    0x4E
#define MINIMAL_KEY_O    0x4F
#define MINIMAL_KEY_P    0x50
#define MINIMAL_KEY_Q    0x51
#define MINIMAL_KEY_R    0x52
#define MINIMAL_KEY_S    0x53
#define MINIMAL_KEY_T    0x54
#define MINIMAL_KEY_U    0x55
#define MINIMAL_KEY_V    0x56
#define MINIMAL_KEY_W    0x57
#define MINIMAL_KEY_X    0x58
#define MINIMAL_KEY_Y    0x59
#define MINIMAL_KEY_Z    0x5A

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

/*
 * -----------------------------------------------------------------------------
 * ----| IMPLEMENTATION |-------------------------------------------------------
 * -----------------------------------------------------------------------------
 */
#ifdef MINIMAL_IMPLEMENTATION

#include <stdio.h>

void minimalGetVersion(int* major, int* minor, int* rev)
{
    if (major != NULL) *major = MINIMAL_VERSION_MAJOR;
    if (minor != NULL) *minor = MINIMAL_VERSION_MINOR;
    if (rev != NULL)   *rev = MINIMAL_VERSION_REVISION;
}

#define MINIMAL_CONCAT_VERSION(m, n, r) #m "." #n "." #r
#define MINIMAL_MAKE_VERSION_STR(m, n, r) MINIMAL_CONCAT_VERSION(m, n, r)

const char* minimalGetVersionString()
{
    return MINIMAL_MAKE_VERSION_STR(MINIMAL_VERSION_MAJOR, MINIMAL_VERSION_MINOR, MINIMAL_VERSION_REVISION);
}

/* --------------------------| game loop |------------------------------- */
void minimalRun(MinimalWindow* window, MinimalTickCB on_tick, void* context)
{
    double seconds = 0.0;
    double lastframe = 0.0;
    uint32_t frames = 0;

    MinimalFrameData framedata = {0};

    while (!minimalShouldClose(window))
    {
        double time = minimalGetTime();
        framedata.deltatime = (float)(time - lastframe);
        lastframe = time;

        minimalUpdateInput();
        minimalPollWindowEvents(window);

        on_tick(context, &framedata);

        frames++;
        if ((minimalGetTime() - seconds) > 1.0)
        {
            seconds += 1.0;
            framedata.fps = frames;
            frames = 0;
        }
    }
}

/* --------------------------| context |--------------------------------- */
static MinimalWindow* _current_context;

void minimalSetCurrentContext(MinimalWindow* context) { _current_context = context; }
MinimalWindow* minimalGetCurrentContext()             { return _current_context; }

/* --------------------------| logging |--------------------------------- */
#define MINIMAL_LOG_BLACK       "\x1b[30m"
#define MINIMAL_LOG_RED         "\x1b[31m"
#define MINIMAL_LOG_GREEN       "\x1b[32m"
#define MINIMAL_LOG_YELLOW      "\x1b[33m"
#define MINIMAL_LOG_BLUE        "\x1b[34m"
#define MINIMAL_LOG_MAGENTA     "\x1b[35m"
#define MINIMAL_LOG_CYAN        "\x1b[36m"
#define MINIMAL_LOG_WHITE       "\x1b[37m"

#define MINIMAL_LOG_BG_BLACK    "\x1b[40m"
#define MINIMAL_LOG_BG_RED      "\x1b[41m"
#define MINIMAL_LOG_BG_GREEN    "\x1b[42m"
#define MINIMAL_LOG_BG_YELLOW   "\x1b[43m"
#define MINIMAL_LOG_BG_BLUE     "\x1b[44m"
#define MINIMAL_LOG_BG_MAGENTA  "\x1b[45m"
#define MINIMAL_LOG_BG_CYAN     "\x1b[46m"
#define MINIMAL_LOG_BG_WHITE    "\x1b[47m"

#define MINIMAL_LOG_RESET       "\x1b[0m" /* no color */

static const char* minimalLoggerGetLevelStr(MinimalLogLevel level)
{
    switch (level)
    {
    case MINIMAL_LOG_TRACE:     return MINIMAL_LOG_WHITE "[TRACE]" MINIMAL_LOG_RESET " ";
    case MINIMAL_LOG_INFO:      return MINIMAL_LOG_GREEN "[INFO]" MINIMAL_LOG_RESET " ";
    case MINIMAL_LOG_WARN:      return MINIMAL_LOG_YELLOW "[WARN]" MINIMAL_LOG_RESET " ";
    case MINIMAL_LOG_ERROR:     return MINIMAL_LOG_RED "[ERROR]" MINIMAL_LOG_RESET " ";
    case MINIMAL_LOG_CRITICAL:  return MINIMAL_LOG_WHITE MINIMAL_LOG_BG_RED "[CRITICAL]" MINIMAL_LOG_RESET " ";
    default: return "";
    }
}

void minimalLoggerPrint(MinimalLogLevel level, const char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    minimalLoggerPrintV(level, fmt, arg);
    va_end(arg);
}

void minimalLoggerPrintV(MinimalLogLevel level, const char* fmt, va_list args)
{
    fprintf(stderr, "%s", minimalLoggerGetLevelStr(level));
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}




typedef struct
{
    uint8_t keys[MINIMAL_KEY_LAST + 1];
    uint8_t prev_keys[MINIMAL_KEY_LAST + 1];

    uint8_t buttons[MINIMAL_MOUSE_BUTTON_LAST + 1];
    uint8_t prev_buttons[MINIMAL_MOUSE_BUTTON_LAST + 1];

    float cursorX, cursorY;
} MinimalInputState;

static MinimalInputState state = { 0 };

void minimalUpdateInput()
{
    MINIMAL_MEMCPY(&state.prev_keys, &state.keys, MINIMAL_KEY_LAST + 1);
    MINIMAL_MEMCPY(&state.prev_buttons, &state.buttons, MINIMAL_MOUSE_BUTTON_LAST + 1);
}

uint8_t minimalProcessKey(MinimalKeycode keycode, uint8_t action)
{
    if (minimalKeycodeValid(keycode) && state.keys[keycode] != action)
    {
        state.keys[keycode] = action;
        return MINIMAL_OK;
    }

    return MINIMAL_FAIL;
}

uint8_t minimalProcessMouseButton(MinimalMouseButton button, uint8_t action)
{
    if (minimalMouseButtonValid(button) && state.buttons[button] != action)
    {
        state.buttons[button] = action;
        return MINIMAL_OK;
    }

    return MINIMAL_FAIL;
}

uint8_t minimalProcessMouseMove(float x, float y)
{
    state.cursorX = x;
    state.cursorY = y;
    return MINIMAL_OK;
}

uint8_t minimalKeycodeValid(MinimalKeycode keycode)
{
    return keycode >= MINIMAL_KEY_FIRST && keycode <= MINIMAL_KEY_LAST;
}

uint8_t minimalMouseButtonValid(MinimalMouseButton button)
{
    return button >= MINIMAL_MOUSE_BUTTON_1 && button <= MINIMAL_MOUSE_BUTTON_LAST;
}

uint8_t minimalKeyPressed(MinimalKeycode keycode)
{
    if (state.keys[keycode])
    {
        MINIMAL_INFO("State: %d", state.keys[keycode]);
        MINIMAL_INFO("Prev:  %d", state.prev_keys[keycode]);
    }
    if (!minimalKeycodeValid(keycode)) return 0;
    return state.keys[keycode] && !state.prev_keys[keycode];
}

uint8_t minimalKeyReleased(MinimalKeycode keycode)
{
    if (!minimalKeycodeValid(keycode)) return 0;
    return state.prev_keys[keycode] && !state.keys[keycode];
}

uint8_t minimalKeyDown(MinimalKeycode keycode)
{
    if (!minimalKeycodeValid(keycode)) return 0;
    return state.keys[keycode];
}

uint8_t minimalKeyModActive(uint32_t keymod)
{
    if (keymod == 0) return 1;

    if ((keymod & MINIMAL_KEY_MOD_SHIFT) && !state.keys[MINIMAL_KEY_SHIFT])
        return 0;
    if ((keymod & MINIMAL_KEY_MOD_CONTROL) && !state.keys[MINIMAL_KEY_CONTROL])
        return 0;
    if ((keymod & MINIMAL_KEY_MOD_ALT) && !state.keys[MINIMAL_KEY_ALT])
        return 0;
    if ((keymod & MINIMAL_KEY_MOD_COMMAND) 
        && !(state.keys[MINIMAL_KEY_LCOMMAND] || state.keys[MINIMAL_KEY_RCOMMAND]))
        return 0;

    return 1;
}


uint8_t minimalMousePressed(MinimalMouseButton button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return state.buttons[button] && !state.prev_buttons[button];
}

uint8_t minimalMouseReleased(MinimalMouseButton button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return state.prev_buttons[button] && !state.buttons[button];
}

uint8_t minimalMouseDown(MinimalMouseButton button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return state.buttons[button];
}

void minimalCursorPos(float* x, float* y)
{
    if (x) *x = state.cursorX;
    if (y) *y = state.cursorY;
}

float minimalCursorX() { return state.cursorX; }
float minimalCursorY() { return state.cursorY; }



#define MINIMAL_LOWORD(dw) ((uint16_t)(dw))
#define MINIMAL_HIWORD(dw) ((uint16_t)(((uint32_t)(dw)) >> 16))

struct MinimalEvent
{
    uint32_t type;
    union
    {
        struct
        {
            uint32_t uParam;
            int32_t lParam;
            int32_t rParam;
        };
        const void* external;
    };
};

static struct
{
    void* context;
    MinimalEventCB callback;
} event_handler;

void minimalSetEventHandler(void* context, MinimalEventCB callback)
{
    event_handler.context = context;
    event_handler.callback = callback;
}

void minimalDispatchEvent(uint32_t type, uint32_t uParam, int32_t lParam, int32_t rParam)
{
    MinimalEvent e = { .type = type, .uParam = uParam, .lParam = lParam, .rParam = rParam };
    if (event_handler.callback) event_handler.callback(event_handler.context, &e);
}

void minimalDispatchExternalEvent(uint32_t type, const void* data)
{
    MinimalEvent e = { .type = type, .external = data };
    if (event_handler.callback) event_handler.callback(event_handler.context, &e);
}

uint8_t minimalEventIsType(const MinimalEvent* e, uint32_t type)  { return e->type == type; }
uint8_t minimalEventIsExternal(const MinimalEvent* e)        { return e->type > MINIMAL_EVENT_LAST; }

const void* minimalExternalEvent(const MinimalEvent* e)
{
    return minimalEventIsExternal(e) ? e->external : NULL;
}

uint8_t minimalEventWindowSize(const MinimalEvent* e, uint32_t* w, uint32_t* h)
{
    if (!minimalEventIsType(e, MINIMAL_EVENT_WINDOW_SIZE)) return 0;

    if (w) *w = (uint32_t)e->lParam;
    if (h) *h = (uint32_t)e->rParam;

    return 1;
}

uint8_t minimalEventMouseButton(const MinimalEvent* e, MinimalMouseButton button, float* x, float* y)
{
    if (!minimalEventIsType(e, MINIMAL_EVENT_MOUSE_BUTTON)) return 0;

    if (button != (MinimalMouseButton)MINIMAL_HIWORD(e->uParam)) return 0;

    if (x) *x = (float)e->lParam;
    if (y) *y = (float)e->rParam;

    return 1;
}

uint8_t minimalEventMouseButtonPressed(const MinimalEvent* e, MinimalMouseButton button, float* x, float* y)
{
    return minimalEventMouseButton(e, button, x, y) && MINIMAL_LOWORD(e->uParam) == MINIMAL_PRESS;
}

uint8_t minimalEventMouseButtonReleased(const MinimalEvent* e, MinimalMouseButton button, float* x, float* y)
{
    return minimalEventMouseButton(e, button, x, y) && MINIMAL_LOWORD(e->uParam) == MINIMAL_RELEASE;
}

uint16_t minimalEventMouseButtonAction(const MinimalEvent* e)
{
    return MINIMAL_LOWORD(e->uParam);
}

uint8_t minimalEventMouseMoved(const MinimalEvent* e, float* x, float* y)
{
    if (!minimalEventIsType(e, MINIMAL_EVENT_MOUSE_MOVED)) return 0;

    if (x) *x = (float)e->lParam;
    if (y) *y = (float)e->rParam;

    return 1;
}

uint8_t minimalEventMouseScrolled(const MinimalEvent* e, float* xoffset, float* yoffset)
{
    if (!minimalEventIsType(e, MINIMAL_EVENT_MOUSE_SCROLLED)) return 0;

    if (xoffset) *xoffset = (float)e->lParam;
    if (yoffset) *yoffset = (float)e->rParam;

    return 1;
}

MinimalKeycode minimalEventKey(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_KEY) ? e->uParam : MINIMAL_KEY_UNKNOWN;
}

MinimalKeycode minimalEventKeyPressed(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_KEY && e->lParam == MINIMAL_PRESS) ? e->uParam : MINIMAL_KEY_UNKNOWN;
}

MinimalKeycode minimalEventKeyReleased(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_KEY && e->lParam == MINIMAL_RELEASE) ? e->uParam : MINIMAL_KEY_UNKNOWN;
}

char minimalEventChar(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_CHAR) ? (char)e->uParam : '\0';
}



#ifdef MINIMAL_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define MINIMAL_WNDCLASSNAME L"MINIMALWNDCLASS"

static uint64_t _minimalTimerFrequency = 0;
static uint64_t _minimalTimerOffset = 0;

static LRESULT minimalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#if defined(MINIMAL_OPENGL)
static uint8_t minimalWGLInit();
static void minimalWGLTerminate();
static HGLRC minimalCreateRenderContext(HDC dc);
#endif

uint8_t minimalPlatformInit()
{
    // register window class
    WNDCLASSEXW wndClass = {
        .cbSize         = sizeof(WNDCLASSEXW),
        .hInstance      = GetModuleHandleW(NULL),
        .lpfnWndProc    = minimalWindowProc,
        .style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpszClassName  = MINIMAL_WNDCLASSNAME,
        .lpszMenuName   = NULL,
        .hIcon          = LoadIconW(NULL, IDI_APPLICATION),
        .hIconSm        = LoadIconW(NULL, IDI_APPLICATION),
        .hCursor        = LoadCursorW(NULL, IDC_ARROW)
    };

    if (!RegisterClassExW(&wndClass))
    {
        MINIMAL_ERROR("[Platform] Failed to register WindowClass");
        return FALSE;
    }

    // init time
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)&_minimalTimerFrequency))
    {
        MINIMAL_ERROR("[Platform] High-resolution performance counter is not supported");
        return MINIMAL_FAIL;
    }

    QueryPerformanceCounter((LARGE_INTEGER*)&_minimalTimerOffset);

#if defined(MINIMAL_OPENGL)
    // init wgl
    if (!minimalWGLInit())
    {
        MINIMAL_ERROR("[Platform] Failed to initialize WGL");
        return MINIMAL_FAIL;
    }
#endif

    return MINIMAL_OK;
}

uint8_t minimalPlatformTerminate()
{
#if defined(MINIMAL_OPENGL)
    minimalWGLTerminate();
#endif

    // unregister window class
    if (!UnregisterClassW(MINIMAL_WNDCLASSNAME, GetModuleHandleW(NULL)))
    {
        MINIMAL_ERROR("[Platform] Failed to unregister WindowClass");
        return MINIMAL_FAIL;
    }
    return MINIMAL_OK;
}

static int32_t windowHints[MINIMAL_HINT_MAX_ENUM];

void minimalSetWindowHint(MinimalWindowHint name, int32_t value)
{
    if (name >= 0 && name < MINIMAL_HINT_MAX_ENUM)
        windowHints[name] = value;
}

struct MinimalWindow
{
    HWND handle;

#ifndef MINIMAL_NO_CONTEXT
    HDC         deviceContext;
    HGLRC       renderContext;
#endif

    uint8_t shouldClose;
};

MinimalWindow* minimalCreateWindow(const char* title, int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    MinimalWindow* window = malloc(sizeof(MinimalWindow));
    if (!window) return NULL;

    // create window
    HINSTANCE instance = GetModuleHandleW(NULL);

    RECT rect = { 0 };
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    DWORD styleEx = WS_EX_APPWINDOW;
    AdjustWindowRectEx(&rect, style, 0, styleEx);

    x += rect.left;
    y += rect.top;
    w += rect.right - rect.left;
    h += rect.bottom - rect.top;

    window->handle = CreateWindowExW(styleEx, MINIMAL_WNDCLASSNAME, NULL, style, x, y, w, h, 0, 0, instance, 0);
    if (!window->handle)
    {
        MINIMAL_ERROR("[Platform] Failed to create window");
        minimalDestroyWindow(window);
        return NULL;
    }

    minimalSetWindowTitle(window, title);

    window->shouldClose = 0;

#ifndef MINIMAL_NO_CONTEXT

    window->deviceContext = GetDC(window->handle);
    window->renderContext = minimalCreateRenderContext(window->deviceContext);
    if (!window->renderContext)
    {
        minimalDestroyWindow(window);
        return NULL;
    }

#endif

    return window;
}

void minimalDestroyWindow(MinimalWindow* window)
{
#ifndef MINIMAL_NO_CONTEXT

    // destroy render context
    if (window->renderContext)
    {
        if (!wglMakeCurrent(NULL, NULL))
        {
            MINIMAL_ERROR("[WGL] Failed to realease render context");
        }

        if (!wglDeleteContext(window->renderContext))
        {
            MINIMAL_ERROR("[WGL] Failed to delete render context");
        }
    }

    // destroy device context
    if (window->deviceContext && !ReleaseDC(window->handle, window->deviceContext))
    {
        MINIMAL_ERROR("[Platform] Failed to release device context");
    }

#endif

    // destroy window
    if (window->handle && !DestroyWindow(window->handle))
    {
        MINIMAL_ERROR("[Platform] Failed to destroy window");
    }

    free(window);
}

void minimalPollWindowEvents(MinimalWindow* context)
{
    MSG msg;
    while (PeekMessageW(&msg, context->handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void minimalSetWindowTitle(MinimalWindow* context, const char* title)
{
    SetWindowTextA(context->handle, title);
}


uint8_t minimalShouldClose(const MinimalWindow* context) { return context->shouldClose; }
void    minimalClose(MinimalWindow* context)             { context->shouldClose = 1; }

double minimalGetTime()
{
    uint64_t value;
    QueryPerformanceCounter((LARGE_INTEGER*)&value);

    return (double)(value - _minimalTimerOffset) / _minimalTimerFrequency;
}

void minimalGetFramebufferSize(const MinimalWindow* context, int32_t* w, int32_t* h)
{
    RECT rect;
    if (GetClientRect(context->handle, &rect))
    {
        if (w) *w = rect.right - rect.left;
        if (h) *h = rect.bottom - rect.top;
    }
}

void minimalGetWindowContentScale(const MinimalWindow* context, float* xscale, float* yscale)
{
    const HDC dc = GetDC(NULL);
    UINT xdpi = GetDeviceCaps(dc, LOGPIXELSX);
    UINT ydpi = GetDeviceCaps(dc, LOGPIXELSY);
    ReleaseDC(NULL, dc);

    if (xscale) *xscale = xdpi / (float)USER_DEFAULT_SCREEN_DPI;
    if (yscale) *yscale = ydpi / (float)USER_DEFAULT_SCREEN_DPI;
}

void* minimalGetNativeWindowHandle(const MinimalWindow* window)
{
    return window->handle;
}


static uint32_t minimalGetKeyMods()
{
    uint32_t mods = 0;
    if (GetKeyState(VK_SHIFT) & 0x8000)                         mods |= MINIMAL_KEY_MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) & 0x8000)                       mods |= MINIMAL_KEY_MOD_CONTROL;
    if (GetKeyState(VK_MENU) & 0x8000)                          mods |= MINIMAL_KEY_MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) mods |= MINIMAL_KEY_MOD_COMMAND;
    if (GetKeyState(VK_CAPITAL) & 1)                            mods |= MINIMAL_KEY_MOD_CAPS_LOCK;
    if (GetKeyState(VK_NUMLOCK) & 1)                            mods |= MINIMAL_KEY_MOD_NUM_LOCK;
    return mods;
}

static uint32_t minimalGetMouseButton(UINT msg, WPARAM wParam)
{
    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)   return MINIMAL_MOUSE_BUTTON_LEFT;
    if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)   return MINIMAL_MOUSE_BUTTON_RIGHT;
    if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)   return MINIMAL_MOUSE_BUTTON_MIDDLE;
    if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)         return MINIMAL_MOUSE_BUTTON_4;
    else                                                return MINIMAL_MOUSE_BUTTON_5;
}

#define MINIMAL_GET_X_LPARAM(lp)    ((int32_t)(int16_t)LOWORD(lp))
#define MINIMAL_GET_Y_LPARAM(lp)    ((int32_t)(int16_t)HIWORD(lp))

#define MINIMAL_GET_SCROLL(wp)      ((int32_t)((int16_t)HIWORD(wp) / (float)WHEEL_DELTA))

static LRESULT minimalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MinimalWindow* context = minimalGetCurrentContext();

    if (!context) return DefWindowProcW(hwnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_QUIT:
    case WM_CLOSE:
        minimalClose(context);
        return 0;
    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_UNICHAR:
    {
        uint32_t codepoint = (uint32_t)wParam;
        uint32_t mods = minimalGetKeyMods();

        if (codepoint > 31)
            minimalDispatchEvent(MINIMAL_EVENT_CHAR, codepoint, 0, mods);

        return 0;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        uint8_t action = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
        uint32_t keycode = (uint16_t)wParam;
        uint32_t mods = minimalGetKeyMods();

        if (minimalProcessKey(keycode, action))
            minimalDispatchEvent(MINIMAL_EVENT_KEY, keycode, action, mods);

        return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    {
        uint32_t action = msg == WM_LBUTTONDOWN
                        || msg == WM_RBUTTONDOWN
                        || msg == WM_MBUTTONDOWN
                        || msg == WM_XBUTTONDOWN;

        uint16_t button = minimalGetMouseButton(msg, wParam);
        int32_t x = MINIMAL_GET_X_LPARAM(lParam);
        int32_t y = MINIMAL_GET_Y_LPARAM(lParam);

        if (minimalProcessMouseButton(button, action))
            minimalDispatchEvent(MINIMAL_EVENT_MOUSE_BUTTON, (button << 16) + action, x, y);

        return msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP;
    }
    case WM_MOUSEMOVE:
    {
        int32_t x = MINIMAL_GET_X_LPARAM(lParam);
        int32_t y = MINIMAL_GET_Y_LPARAM(lParam);

        if (minimalProcessMouseMove((float)x, (float)y))
            minimalDispatchEvent(MINIMAL_EVENT_MOUSE_MOVED, 0, x, y);
        return 0;
    }
    case WM_MOUSEWHEEL:
    {
        int32_t scroll = MINIMAL_GET_SCROLL(wParam);

        minimalDispatchEvent(MINIMAL_EVENT_MOUSE_SCROLLED, 0, 0, scroll);
        return 0;
    }
    case WM_MOUSEHWHEEL:
    {
        int32_t scroll = MINIMAL_GET_SCROLL(wParam);

        minimalDispatchEvent(MINIMAL_EVENT_MOUSE_SCROLLED, 0, scroll, 0);
        return 0;
    }
    case WM_SIZE:
    {
        /*
        uint8_t minimized = wParam == SIZE_MINIMIZED;
        uint8_t maximized = wParam == SIZE_MAXIMIZED || (_current_context->maximized && wParam != SIZE_RESTORED);

        if (_current_context->minimized != minimized)
        {
            _current_context->minimized = minimized;
            minimalDispatchEvent(event_handler, MINIMAL_EVENT_WINDOW_MINIMIZE, minimized, 0, 0);
        }

        if (_current_context->maximized != maximized)
        {
            _current_context->maximized = maximized;
            minimalDispatchEvent(event_handler, MINIMAL_EVENT_WINDOW_MAXIMIZE, maximized, 0, 0);
        }
        */

        int32_t width  = LOWORD(lParam);
        int32_t height = HIWORD(lParam);

        minimalDispatchEvent(MINIMAL_EVENT_WINDOW_SIZE, 0, width, height);
        return 0;
    }
    default: return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

/* --------------------------| wgl |------------------------------------- */
#ifndef MINIMAL_NO_CONTEXT

/* Accepted as an attribute name in <*attribList> */
#define WGL_CONTEXT_MAJOR_VERSION_ARB               0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB               0x2092
#define WGL_CONTEXT_FLAGS_ARB                       0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB                0x9126

/* Accepted as bits in the attribute value for WGL_CONTEXT_FLAGS in <*attribList> */
#define WGL_CONTEXT_DEBUG_BIT_ARB                   0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB      0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB            0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB   0x00000002

// See https://www.opengl.org/registry/specs/ARB/wgl_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                      0x2001
#define WGL_ACCELERATION_ARB                        0x2003
#define WGL_SUPPORT_OPENGL_ARB                      0x2010
#define WGL_DOUBLE_BUFFER_ARB                       0x2011
#define WGL_PIXEL_TYPE_ARB                          0x2013
#define WGL_COLOR_BITS_ARB                          0x2014
#define WGL_DEPTH_BITS_ARB                          0x2022
#define WGL_STENCIL_BITS_ARB                        0x2023

#define WGL_FULL_ACCELERATION_ARB                   0x2027
#define WGL_TYPE_RGBA_ARB                           0x202B


// WGL extension pointer typedefs
typedef BOOL(WINAPI* wglSwapIntervalEXT_T)(int);
typedef BOOL(WINAPI* wglChoosePixelFormatARB_T)(HDC, const int*, const float*, UINT, int*, UINT*);
typedef HGLRC(WINAPI* wglCreateContextAttribsARB_T)(HDC, HGLRC, const int*);

static wglCreateContextAttribsARB_T _wglCreateContextAttrARB = NULL;
static wglSwapIntervalEXT_T         _wglSwapIntervalEXT = NULL;
static wglChoosePixelFormatARB_T    _wglChoosePixelFormatARB = NULL;

static HMODULE glModule = NULL;

uint8_t minimalWGLInit()
{
    // load opengl library
    glModule = LoadLibraryA("opengl32.dll");
    if (!glModule)
    {
        MINIMAL_ERROR("[WGL] Failed to load opengl32.dll");
        return MINIMAL_FAIL;
    }

    // create helper window
    HINSTANCE instance = GetModuleHandleW(NULL);
    LPCWSTR className = MINIMAL_WNDCLASSNAME;
    DWORD exStyle = WS_EX_OVERLAPPEDWINDOW;
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    HWND helperWnd = CreateWindowExW(exStyle, className, NULL, style, 0, 0, 1, 1, 0, 0, instance, 0);

    if (!helperWnd)
    {
        MINIMAL_ERROR("[WGL] Failed to create helper window");
        return MINIMAL_FAIL;
    }

    MSG msg;
    while (PeekMessageW(&msg, helperWnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    HDC dc = GetDC(helperWnd);

    PIXELFORMATDESCRIPTOR pfd = {
        .nSize = sizeof(pfd),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 24,
    };

    if (!SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd))
    {
        MINIMAL_ERROR("[WGL] Failed to set pixel format for dummy context");
        return MINIMAL_FAIL;
    }

    HGLRC rc = wglCreateContext(dc);
    if (!rc)
    {
        MINIMAL_ERROR("[WGL] Failed to create dummy context");
        return MINIMAL_FAIL;
    }

    HDC   pdc = wglGetCurrentDC();
    HGLRC prc = wglGetCurrentContext();

    if (!wglMakeCurrent(dc, rc))
    {
        MINIMAL_ERROR("[WGL] Failed to make dummy context current");
        wglMakeCurrent(pdc, prc);
        wglDeleteContext(rc);
        return MINIMAL_FAIL;
    }

    _wglCreateContextAttrARB =  (wglCreateContextAttribsARB_T)wglGetProcAddress("wglCreateContextAttribsARB");
    _wglSwapIntervalEXT =       (wglSwapIntervalEXT_T)wglGetProcAddress("wglSwapIntervalEXT");
    _wglChoosePixelFormatARB =  (wglChoosePixelFormatARB_T)wglGetProcAddress("wglChoosePixelFormatARB");

    wglMakeCurrent(pdc, prc);
    wglDeleteContext(rc);

    // destroy helper window
    if (!DestroyWindow(helperWnd))
    {
        MINIMAL_ERROR("[WGL] Failed to destroy helper window");
        return MINIMAL_FAIL;
    }

    return MINIMAL_OK;
}

void minimalWGLTerminate()
{
    if (glModule)
        FreeLibrary(glModule);
}

HGLRC minimalCreateRenderContext(HDC dc)
{
    int pfAttribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     1,
        WGL_SUPPORT_OPENGL_ARB,     1,
        WGL_DOUBLE_BUFFER_ARB,      1,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        0
    };

    int format;
    UINT formatCount;
    _wglChoosePixelFormatARB(dc, pfAttribs, 0, 1, &format, &formatCount);
    if (!formatCount)
    {
        MINIMAL_ERROR("[WGL] Could not find a suitable pixel format");
        return NULL;
    }

    PIXELFORMATDESCRIPTOR pfd;
    if (!DescribePixelFormat(dc, format, sizeof(pfd), &pfd))
    {
        MINIMAL_ERROR("[WGL] Failed to retrieve PFD");
        return NULL;
    }

    if (!SetPixelFormat(dc, format, &pfd))
    {
        MINIMAL_ERROR("[WGL] Failed to set pixel format");
        return NULL;
    }

    int glAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,  windowHints[MINIMAL_HINT_CONTEXT_MAJOR_VERSION],
        WGL_CONTEXT_MINOR_VERSION_ARB,  windowHints[MINIMAL_HINT_CONTEXT_MINOR_VERSION],
        WGL_CONTEXT_PROFILE_MASK_ARB,   WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef _DEBUG
        WGL_CONTEXT_FLAGS_ARB,          WGL_CONTEXT_DEBUG_BIT_ARB,
#endif // _DEBUG
        0,
    };

    HGLRC rc = _wglCreateContextAttrARB(dc, 0, glAttribs);
    if (!rc)
    {
        MINIMAL_ERROR("[WGL] Failed to create render context");
        return MINIMAL_FAIL;
    }

    if (!wglMakeCurrent(dc, rc))
    {
        MINIMAL_ERROR("[WGL] Failed to make render context current");
        return MINIMAL_FAIL;
    }

    return rc;
}

void* minimalGetGLProcAddress(const char* name)
{
    void* p = (void*)wglGetProcAddress(name);
    if (p) return p;

    return (void*)GetProcAddress(glModule, name);;
}

void minimalSwapBuffers(MinimalWindow* context)
{
    SwapBuffers(context->deviceContext);
}

void minimalSwapInterval(uint8_t interval)
{
    if (_wglSwapIntervalEXT) _wglSwapIntervalEXT(interval);
}

#endif // !MINIMAL_NO_CONTEXT

#endif // MINIMAL_PLATFORM_WINDOWS

#endif /* !MINIMAL_IMPLEMENTATION */

/*
MIT License

Copyright (c) 2021 oliverjakobs

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/