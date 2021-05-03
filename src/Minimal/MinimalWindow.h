#ifndef MINIMAL_WINDOW_H
#define MINIMAL_WINDOW_H

#include "MinimalUtil.h"
#include "MinimalEvent.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#define MINIMAL_WNDCLASSNAME L"MINIMALWNDCLASS"

MinimalBool MinimalRegisterWindowClass();
MinimalBool MinimalUnregisterWindowClass();

MinimalBool MinimalCreateHelperWindow();
MinimalBool MinimalDestroyHelperWindow();
HWND MinimalGetHelperWindow();

typedef void (*MinimalSizeCB)       (MinimalWindow* wnd, uint32_t w, uint32_t h);
typedef void (*MinimalCloseCB)      (MinimalWindow* wnd);
typedef void (*MinimalKeyCB)        (MinimalWindow* wnd, UINT keycode, UINT scancode, UINT action, UINT mods);
typedef void (*MinimalCharCB)       (MinimalWindow* wnd, UINT codepoint, UINT mods);
typedef void (*MinimalMButtonCB)    (MinimalWindow* wnd, UINT keycode, UINT action, UINT mods, INT x, INT y);
typedef void (*MinimalScrollCB)     (MinimalWindow* wnd, INT h, INT v);
typedef void (*MinimalCursorPosCB)  (MinimalWindow* wnd, INT x, INT y);
typedef void (*MinimalMaximizeCB)   (MinimalWindow* wnd, MinimalBool maximized);
typedef void (*MinimalIconifyCB)    (MinimalWindow* wnd, MinimalBool iconified);

typedef struct
{
    int gl_major;
    int gl_minor;

    int context_flags;
} MinimalWindowConfig;

MinimalWindow* MinimalCreateWindow(const char* title, uint32_t w, uint32_t h, const MinimalWindowConfig* config);
MinimalBool MinimalDestroyWindow(MinimalWindow* window);

void MinimalPollEvent(MinimalWindow* window);
void MinimalSwapBuffer(MinimalWindow* window);

void MinimalSetWindowTitle(MinimalWindow* window, const char* str);
void MinimalSetEventDispatch(MinimalWindow* window, void* handler, MinimalDispatchEventCB dispatch);

uint32_t MinimalGetWindowWidth(const MinimalWindow* window);
uint32_t MinimalGetWindowHeigth(const MinimalWindow* window);

MinimalBool MinimalShouldCloseWindow(const MinimalWindow* window);
void MinimalCloseWindow(MinimalWindow* window);

void MinimalSetSizeCallback(MinimalWindow* window, MinimalSizeCB size);
void MinimalSetCloseCallback(MinimalWindow* window, MinimalCloseCB close);
void MinimalSetKeyCallback(MinimalWindow* window, MinimalKeyCB key);
void MinimalSetCharCallback(MinimalWindow* window, MinimalCharCB character);
void MinimalSetMButtonCallback(MinimalWindow* window, MinimalMButtonCB m_button);
void MinimalSetScrollCallback(MinimalWindow* window, MinimalScrollCB scroll);
void MinimalSetCursorPosCallback(MinimalWindow* window, MinimalCursorPosCB cursor_pos);

/* --------------------------| input |----------------------------------- */
void MinimalCreateKeyTable();

/* tanslate win32 virtual key codes to minimal key codes */
uint32_t MinimalTranslateKey(uint32_t scancode);

void MinimalWindowUpdateKeyStates(MinimalWindow* window);

const MinimalInputState* MinimalWindowGetKeyState(const MinimalWindow* window, uint32_t keycode);
int8_t MinimalWindowGetMouseButtonState(const MinimalWindow* window, uint32_t button);

void MinimalWindowGetCursorPos(const MinimalWindow* window, float* x, float* y);

/* --------------------------| helper |---------------------------------- */
void  MinimalWindowSetContext(MinimalWindow* window, HDC dc, HGLRC rc);
HWND  MinimalWindowGetHandle(const MinimalWindow* window);
HDC   MinimalWindowGetDeviceContext(const MinimalWindow* window);
HGLRC MinimalWindowGetRenderContext(const MinimalWindow* window);

#endif // !MINIMAL_WINDOW_H
