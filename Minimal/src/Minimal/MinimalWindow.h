#ifndef MINIMAL_WINDOW_H
#define MINIMAL_WINDOW_H

#include "MinimalUtil.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

typedef struct
{
    HWND        handle;
    HDC         device_context;
    HGLRC       render_context;
    HINSTANCE   instance;

    int key_state[280];

    int should_close;
} MinimalWindow;

int MinimalCreateWindow(MinimalWindow* window, const char* title, int width, int height);
int MinimalDestroyWindow(MinimalWindow* window);

void MinimalPollEvent(MinimalWindow* window);
void MinimalSwapBuffer(MinimalWindow* window);

void MinimalSetWindowTitle(MinimalWindow* window, const char* str);

int MinimalShouldClose(const MinimalWindow* window);
void MinimalCloseWindow(MinimalWindow* window);

#endif // !MINIMAL_WINDOW_H
