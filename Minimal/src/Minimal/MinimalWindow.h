#ifndef MINIMAL_WINDOW_H
#define MINIMAL_WINDOW_H

#include "MinimalUtil.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

struct MinimalWindow
{
    HINSTANCE   instance;
    HWND        handle;
    HDC         device_context;
    HGLRC       render_context;

    int key_state[280];

    int should_close;

    /* callbacks */
    struct
    {
        void (*size)            (MinimalWindow* wnd, long w, long h);
        void (*close)           (MinimalWindow* wnd);
        void (*key)             (MinimalWindow* wnd, UINT keycode, UINT scancode, UINT action, UINT mods);
        void (*character)       (MinimalWindow* wnd, UINT keycode);
        void (*mouse_button)    (MinimalWindow* wnd, UINT keycode, UINT scancode, UINT action, UINT mods);
        void (*scroll)          (MinimalWindow* wnd, float x_offset, float y_offset);
        void (*cursor_pos)      (MinimalWindow* wnd, float x_pos, float y_pos);
    } callbacks;
};

int MinimalCreateWindow(MinimalWindow* window, const char* title, int width, int height);
int MinimalDestroyWindow(MinimalWindow* window);

void MinimalPollEvent(MinimalWindow* window);
void MinimalSwapBuffer(MinimalWindow* window);

void MinimalSetWindowTitle(MinimalWindow* window, const char* str);

long MinimalGetWindowWidth(const MinimalWindow* window);
long MinimalGetWindowHeigth(const MinimalWindow* window);

int MinimalShouldClose(const MinimalWindow* window);
void MinimalCloseWindow(MinimalWindow* window);

#endif // !MINIMAL_WINDOW_H
