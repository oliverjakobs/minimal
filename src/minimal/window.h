#ifndef MINIMAL_WINDOW_H
#define MINIMAL_WINDOW_H

#include "utils.h"
#include "event.h"

#include "platform.h"

typedef void (*MinimalSizeCB)       (MinimalWindow* wnd, uint32_t w, uint32_t h);
typedef void (*MinimalCloseCB)      (MinimalWindow* wnd);
typedef void (*MinimalKeyCB)        (MinimalWindow* wnd, uint32_t keycode, uint32_t scancode, uint32_t action, uint32_t mods);
typedef void (*MinimalCharCB)       (MinimalWindow* wnd, uint32_t codepoint, uint32_t mods);
typedef void (*MinimalMButtonCB)    (MinimalWindow* wnd, uint32_t keycode, uint32_t action, uint32_t mods, int32_t x, int32_t y);
typedef void (*MinimalScrollCB)     (MinimalWindow* wnd, int32_t h, int32_t v);
typedef void (*MinimalCursorPosCB)  (MinimalWindow* wnd, int32_t x, int32_t y);
typedef void (*MinimalMaximizeCB)   (MinimalWindow* wnd, int8_t maximized);
typedef void (*MinimalMinimizeCB)   (MinimalWindow* wnd, int8_t minimized);

typedef struct
{
    int gl_major;
    int gl_minor;

    int flags;
} MinimalWindowConfig;

MinimalWindow* minimalCreateWindow(const char* title, uint32_t w, uint32_t h, const MinimalWindowConfig* config);
uint8_t minimalDestroyWindow(MinimalWindow* window);

void minimalMakeContextCurrent(MinimalWindow* window);
MinimalWindow* minimalGetCurrentContext();

void minimalPollEvent(MinimalWindow* window);
void minimalSwapBuffer(MinimalWindow* window);

void minimalSetWindowTitle(MinimalWindow* window, const char* title);
void minimalWindowSetApp(MinimalWindow* window, MinimalApp* app);

uint32_t minimalGetWindowWidth(const MinimalWindow* window);
uint32_t minimalGetWindowHeigth(const MinimalWindow* window);

uint8_t minimalShouldCloseWindow(const MinimalWindow* window);
void    minimalCloseWindow(MinimalWindow* window);

void minimalWindowMinimize(MinimalWindow* window, uint8_t minimized);
void minimalWindowMaximize(MinimalWindow* window, uint8_t maximized);

uint8_t minimalWindowIsMinimized(const MinimalWindow* window);
uint8_t minimalWindowIsMaximized(const MinimalWindow* window);

/* --------------------------| input |----------------------------------- */
int8_t minimalGetKeyState(const MinimalWindow* window, uint32_t keycode);
int8_t minimalGetMouseButtonState(const MinimalWindow* window, uint32_t button);

void minimalGetCursorPos(const MinimalWindow* window, float* x, float* y);

/* --------------------------| helper |---------------------------------- */
void MinimalCallChar(MinimalWindow* window, uint32_t codepoint, uint32_t mods);
void MinimalCallKey(MinimalWindow* window, uint32_t key, uint32_t scancode, uint32_t action, uint32_t mods);
void MinimalCallMouseButton(MinimalWindow* window, uint32_t button, uint32_t action, uint32_t mods, int32_t x, int32_t y);
void MinimalCallCursorPos(MinimalWindow* window, uint32_t x, uint32_t y);
void MinimalCallMouseScroll(MinimalWindow* window, int32_t h, int32_t v);
void MinimalCallWindowSize(MinimalWindow* window, uint32_t w, uint32_t h);
void MinimalCallWindowMinimize(MinimalWindow* window, int8_t minimized);
void MinimalCallWindowMaximize(MinimalWindow* window, int8_t maximized);

#endif // !MINIMAL_WINDOW_H
