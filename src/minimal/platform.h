#ifndef MINIMAL_PLATFORM_H
#define MINIMAL_PLATFORM_H

#include "common.h"

#define MINIMAL_CONTEXT_DEBUG_BIT               0x0001
#define MINIMAL_CONTEXT_FORWARD_COMPATIBLE_BIT  0x0002

#define MINIMAL_CONTEXT_CORE_PROFILE            0x00000001
#define MINIMAL_CONTEXT_COMPATIBILITY_PROFILE   0x00000002

// platforms
//#define MINIMAL_PLATFORM_WINDOWS
//#define MINIMAL_PLATFORM_GLFW

#ifdef MINIMAL_PLATFORM_WINDOWS
#undef MINIMAL_PLATFORM_GLFW
#endif // MINIMAL_PLATFORM_WINDOWS

#ifdef MINIMAL_PLATFORM_GLFW
#undef MINIMAL_PLATFORM_WINDOWS
#endif // MINIMAL_PLATFORM_GLFW

uint8_t minimalPlatformInit();
uint8_t minimalPlatformTerminate();

typedef struct
{
    int gl_major;
    int gl_minor;

    int profile;
    int flags;
} MinimalWndConfig;

MinimalWindow* minimalCreateWindow(const char* title, uint32_t w, uint32_t h, const MinimalWndConfig* config);
void minimalDestroyWindow(MinimalWindow* window);

void  minimalSetWindowEventHandler(MinimalWindow* window, void* handler);
void* minimalGetWindowEventHandler(MinimalWindow* window);

void minimalMakeContextCurrent(MinimalWindow* context);
MinimalWindow* minimalGetCurrentContext();

void minimalPollWindowEvents(MinimalWindow* context);
void minimalSwapBuffers(MinimalWindow* context);

void minimalSetWindowTitle(MinimalWindow* context, const char* title);

void minimalSwapInterval(uint8_t interval);

uint8_t minimalShouldClose(const MinimalWindow* context);
void    minimalCloseWindow(MinimalWindow* context);

void minimalGetContentSize(const MinimalWindow* context, int32_t* w, int32_t* h);
void minimalGetWindowSize(const MinimalWindow* context, int32_t* w, int32_t* h);

int8_t minimalGetKeyState(const MinimalWindow* context, uint32_t keycode);
int8_t minimalGetMouseButtonState(const MinimalWindow* context, uint32_t button);

void minimalGetCursorPos(const MinimalWindow* context, float* x, float* y);

double minimalGetTime();

#endif // !MINIMAL_PLATFORM_H
