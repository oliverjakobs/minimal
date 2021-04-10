#ifndef MINIMAL_WINDOW_H
#define MINIMAL_WINDOW_H

#include "MinimalUtil.h"

typedef struct
{
    HWND        handle;
    HDC         device_context;
    HGLRC       render_context;
    HINSTANCE   instance;

    int should_close;
} MinimalWindow;

int MinimalCreateWindow(MinimalWindow* window, LPCWSTR title, int width, int height);
int MinimalDestroyWindow(MinimalWindow* window);

void MinimalPollEvent(MinimalWindow* window);
void MinimalSwapBuffer(MinimalWindow* window);

int MinimalShouldClose(const MinimalWindow* window);
void MinimalCloseWindow(MinimalWindow* window);

#endif // !MINIMAL_WINDOW_H
