#ifndef MINIMAL_PLATFORM_H
#define MINIMAL_PLATFORM_H

#include "utils.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

uint8_t minimalPlatformInit();
uint8_t minimalPlatformTerminate();

typedef struct
{
    HINSTANCE   instance;
    HWND        handle;
    HDC         device_context;
    HGLRC       render_context;
} MinimalPlatform;

uint8_t minimalPlatformCreateWindow(MinimalPlatform* platform, const char* title, uint32_t w, uint32_t h);
uint8_t minimalPlatformDestroyWindow(MinimalPlatform* platform);

uint8_t minimalPlatformCreateRenderContext(MinimalPlatform* platform, int gl_minor, int gl_major, int flags);
uint8_t minimalPlatformDestroyRenderContext(MinimalPlatform* platform);

uint8_t minimalPlatformSetWindowTitle(MinimalPlatform* platform, const char* title);
uint8_t minimalPlatformSwapBuffers(MinimalPlatform* platform);
void    minimalPlatformSwapInterval(uint8_t interval);

void minimalPlatformPollEvent(MinimalPlatform* platform);

int8_t minimalPlatformGetKeyState(const MinimalWindow* window, uint32_t keycode);
int8_t minimalPlatformGetMouseButtonState(const MinimalWindow* window, uint32_t button);

void minimalPlatformGetCursorPos(const MinimalPlatform* platform, float* x, float* y);

double minimalPlatformGetTime();

#endif // !MINIMAL_PLATFORM_H
