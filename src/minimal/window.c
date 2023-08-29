#include "window.h"

#include "platform.h"
#include "input.h"

struct MinimalWindow
{
    MinimalApp* app;
    MinimalPlatform platform;

    uint8_t key_state[MINIMAL_KEY_LAST + 1];
    uint8_t mouse_buttons[MINIMAL_MOUSE_BUTTON_LAST + 1];

    uint8_t should_close : 1;
    uint8_t maximized    : 1;
    uint8_t minimized    : 1;

    uint32_t width;
    uint32_t height;

    /* window callbacks */
    struct
    {
        MinimalSizeCB       size;
        MinimalCloseCB      close;
        MinimalKeyCB        key;
        MinimalCharCB       character;
        MinimalMButtonCB    m_button;
        MinimalScrollCB     scroll;
        MinimalCursorPosCB  cursor_pos;
        MinimalMaximizeCB   maximize;
        MinimalMinimizeCB   minimize;
    } callbacks;
};

MinimalWindow* minimalCreateWindow(const char* title, uint32_t w, uint32_t h, const MinimalWindowConfig* config)
{
    MinimalWindow* wnd = malloc(sizeof(MinimalWindow));

    if (!wnd) return NULL;

    wnd->width = w;
    wnd->height = h;

    wnd->should_close = 0;
    wnd->maximized = 0;
    wnd->minimized = 0;

    wnd->app = NULL;

    memset(wnd->key_state, MINIMAL_RELEASE, sizeof(wnd->key_state));
    memset(wnd->mouse_buttons, MINIMAL_RELEASE, sizeof(wnd->mouse_buttons));

    wnd->callbacks.size = NULL;
    wnd->callbacks.close = NULL;
    wnd->callbacks.key = NULL;
    wnd->callbacks.character = NULL;
    wnd->callbacks.m_button = NULL;
    wnd->callbacks.scroll = NULL;
    wnd->callbacks.cursor_pos = NULL;
    wnd->callbacks.maximize = NULL;
    wnd->callbacks.minimize = NULL;

    minimalPlatformCreateWindow(&wnd->platform, title, w, h);

    if (!minimalPlatformCreateRenderContext(&wnd->platform, config->gl_major, config->gl_minor, config->flags))
    {
        minimalDestroyWindow(wnd);
        wnd = NULL;
    }
    return wnd;
}

uint8_t minimalDestroyWindow(MinimalWindow* wnd)
{
    uint8_t status = MINIMAL_OK;
    if (!minimalPlatformDestroyRenderContext(&wnd->platform))
    {
        MINIMAL_ERROR("[Window] Failed to render context");
        status = MINIMAL_FAIL;
    }

    if (!minimalPlatformDestroyWindow(&wnd->platform))
    {
        MINIMAL_ERROR("[Window] Failed to destroy window");
        status = MINIMAL_FAIL;
    }

    free(wnd);
    return status;
}

static MinimalWindow* _current_context;

void minimalMakeContextCurrent(MinimalWindow* window)   { _current_context = window; }
MinimalWindow* minimalGetCurrentContext()               { return _current_context; }

void minimalPollEvent(MinimalWindow* wnd)                         { minimalPlatformPollEvent(&wnd->platform); }
void minimalSwapBuffer(MinimalWindow* wnd)                        { minimalPlatformSwapBuffers(&wnd->platform); }
void minimalSetWindowTitle(MinimalWindow* wnd, const char* title) { minimalPlatformSetWindowTitle(&wnd->platform, title); }

void minimalWindowSetApp(MinimalWindow* window, MinimalApp* app)
{
    window->app = app;
}

uint32_t minimalGetWindowWidth(const MinimalWindow* wnd)    { return wnd->width; }
uint32_t minimalGetWindowHeigth(const MinimalWindow* wnd)   { return wnd->height; }

uint8_t minimalShouldCloseWindow(const MinimalWindow* wnd)  { return wnd->should_close; }
void    minimalCloseWindow(MinimalWindow* wnd)              { wnd->should_close = 1; }

void    minimalWindowMinimize(MinimalWindow* window, uint8_t minimized) { window->minimized = minimized; }
void    minimalWindowMaximize(MinimalWindow* window, uint8_t maximized) { window->maximized = maximized; }
uint8_t minimalWindowIsMinimized(const MinimalWindow* window)           { return window->minimized; }
uint8_t minimalWindowIsMaximized(const MinimalWindow* window)           { return window->maximized; }

/* --------------------------| input |----------------------------------- */
int8_t minimalGetKeyState(const MinimalWindow* window, uint32_t keycode)
{
    return (window && minimalKeycodeValid(keycode)) ? window->key_state[keycode] : -1;
}

int8_t minimalGetMouseButtonState(const MinimalWindow* window, uint32_t button)
{
    return (window && minimalMouseButtonValid(button)) ? window->mouse_buttons[button] : -1;
}

void minimalGetCursorPos(const MinimalWindow* window, float* x, float* y)
{
    minimalPlatformGetCursorPos(&window->platform, x, y);
}

/* --------------------------| helper |---------------------------------- */
void MinimalCallChar(MinimalWindow* window, uint32_t codepoint, uint32_t mods)
{
    if (window->callbacks.character) window->callbacks.character(window, codepoint, mods);
    minimalDispatchEvent(window->app, MINIMAL_EVENT_CHAR, codepoint, 0, mods);
}

void MinimalCallKey(MinimalWindow* window, uint32_t key, uint32_t scancode, uint32_t action, uint32_t mods)
{
    if (minimalKeycodeValid(key)) window->key_state[key] = (uint8_t)action;

    if (window->callbacks.key) window->callbacks.key(window, key, scancode, action, mods);
    minimalDispatchEvent(window->app, MINIMAL_EVENT_KEY, key, action, mods);
}

void MinimalCallMouseButton(MinimalWindow* window, uint32_t button, uint32_t action, uint32_t mods, int32_t x, int32_t y)
{
    if (minimalMouseButtonValid(button)) window->mouse_buttons[button] = (uint8_t)action;

    if (window->callbacks.m_button) window->callbacks.m_button(window, button, action, mods, x, y);
    minimalDispatchEvent(window->app, MINIMAL_EVENT_MOUSE_BUTTON, (button << 16) + action, x, y);
}

void MinimalCallCursorPos(MinimalWindow* window, uint32_t x, uint32_t y)
{
    if (window->callbacks.cursor_pos) window->callbacks.cursor_pos(window, x, y);
    minimalDispatchEvent(window->app, MINIMAL_EVENT_MOUSE_MOVED, 0, x, y);
}

void MinimalCallMouseScroll(MinimalWindow* window, int32_t h, int32_t v)
{
    if (window->callbacks.scroll) window->callbacks.scroll(window, h, v);
    minimalDispatchEvent(window->app, MINIMAL_EVENT_MOUSE_SCROLLED, 0, h, v);
}

void MinimalCallWindowSize(MinimalWindow* window, uint32_t w, uint32_t h)
{
    window->width = w;
    window->height = h;

    if (window->callbacks.size) window->callbacks.size(window, w, h);
    minimalDispatchEvent(window->app, MINIMAL_EVENT_WINDOW_SIZE, 0, w, h);
}

void MinimalCallWindowMinimize(MinimalWindow* window, int8_t minimized)
{
    if (window->callbacks.minimize) window->callbacks.minimize(window, minimized);
    window->minimized = minimized;
}

void MinimalCallWindowMaximize(MinimalWindow* window, int8_t maximized)
{
    if (window->callbacks.maximize) window->callbacks.maximize(window, maximized);
    window->maximized = maximized;
}