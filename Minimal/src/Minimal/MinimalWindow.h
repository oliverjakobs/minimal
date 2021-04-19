#ifndef MINIMAL_WINDOW_H
#define MINIMAL_WINDOW_H

#include "MinimalUtil.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#define MINIMAL_WNDCLASSNAME L"MINIMALWNDCLASS"

typedef void (*MinimalSizeCB)       (MinimalWindow* wnd, long w, long h);
typedef void (*MinimalCloseCB)      (MinimalWindow* wnd);
typedef void (*MinimalKeyCB)        (MinimalWindow* wnd, UINT keycode, UINT scancode, UINT action, UINT mods);
typedef void (*MinimalCharCB)       (MinimalWindow* wnd, UINT keycode);
typedef void (*MinimalMButtonCB)    (MinimalWindow* wnd, UINT keycode, UINT action, UINT mods);
typedef void (*MinimalScrollCB)     (MinimalWindow* wnd, float x_offset, float y_offset);
typedef void (*MinimalCursorPosCB)  (MinimalWindow* wnd, float x_pos, float y_pos);

struct MinimalWindow
{
    HINSTANCE   instance;
    HWND        handle;
    HDC         device_context;
    HGLRC       render_context;

    uint8_t key_state[MINIMAL_KEY_LAST + 1];
    uint8_t mouse_buttons[MINIMAL_MOUSE_BUTTON_LAST + 1];

    uint8_t should_close;

    /* callbacks */
    struct
    {
        MinimalSizeCB       size;
        MinimalCloseCB      close;
        MinimalKeyCB        key;
        MinimalCharCB       character;
        MinimalMButtonCB    m_button;
        MinimalScrollCB     scroll;
        MinimalCursorPosCB  cursor_pos;
    } callbacks;
};

uint8_t MinimalRegisterWindowClass();
uint8_t MinimalUnregisterWindowClass();

uint8_t MinimalCreateHelperWindow();
HWND MinimalGetHelperWindow();

uint8_t MinimalCreateWindow(MinimalWindow* window, const char* title, uint32_t width, uint32_t height);
uint8_t MinimalDestroyWindow(MinimalWindow* window);

void MinimalPollEvent(MinimalWindow* window);
void MinimalSwapBuffer(MinimalWindow* window);

void MinimalSetWindowTitle(MinimalWindow* window, const char* str);

uint32_t MinimalGetWindowWidth(const MinimalWindow* window);
uint32_t MinimalGetWindowHeigth(const MinimalWindow* window);

uint8_t MinimalShouldCloseWindow(const MinimalWindow* window);
void MinimalCloseWindow(MinimalWindow* window);

void MinimalSetSizeCallback(MinimalWindow* window, MinimalSizeCB size);
void MinimalSetCloseCallback(MinimalWindow* window, MinimalCloseCB close);
void MinimalSetKeyCallback(MinimalWindow* window, MinimalKeyCB key);
void MinimalSetCharCallback(MinimalWindow* window, MinimalCharCB character);
void MinimalSetMButtonCallback(MinimalWindow* window, MinimalMButtonCB m_button);
void MinimalSetScrollCallback(MinimalWindow* window, MinimalScrollCB scroll);
void MinimalSetCursorPosCallback(MinimalWindow* window, MinimalCursorPosCB cursor_pos);

/* --------------------------| Input |----------------------------------- */
void MinimalCreateKeyTable();

/* tanslate win32 virtual key codes to minimal key codes */
uint32_t MinimalTranslateKey(uint32_t scancode);

int8_t MinimalGetKeyState(const MinimalWindow* window, uint32_t keycode);
int8_t MinimalGetMouseButtonState(const MinimalWindow* window, uint32_t button);

#endif // !MINIMAL_WINDOW_H
