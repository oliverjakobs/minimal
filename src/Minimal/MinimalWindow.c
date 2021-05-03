#include "MinimalWindow.h"

#include "MinimalWGL.h"

#define MINIMAL_GET_X_LPARAM(lp)    ((INT)(SHORT)LOWORD(lp))
#define MINIMAL_GET_Y_LPARAM(lp)    ((INT)(SHORT)HIWORD(lp))

#define MINIMAL_GET_SCROLL(wp)      ((INT)((SHORT)HIWORD(wp) / (float)WHEEL_DELTA))

#define MINIMAL_MSG_BUTTON_DOWN(msg) \
    (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN)

struct MinimalWindow
{
    HINSTANCE   instance;
    HWND        handle;
    HDC         device_context;
    HGLRC       render_context;

    MinimalInputState key_state[MINIMAL_KEY_LAST + 1];
    uint8_t mouse_buttons[MINIMAL_MOUSE_BUTTON_LAST + 1];

    MinimalBool should_close;
    MinimalBool maximized;
    MinimalBool iconified;

    uint32_t width;
    uint32_t height;

    /* events */
    void* event_handler;
    MinimalDispatchEventCB dispatch;

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
        MinimalIconifyCB    iconify;
    } callbacks;
};

static uint32_t MinimalGetKeyMods()
{
    uint32_t mods = 0;
    if (GetKeyState(VK_SHIFT) & 0x8000)                         mods |= MINIMAL_KEY_MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) & 0x8000)                       mods |= MINIMAL_KEY_MOD_CONTROL;
    if (GetKeyState(VK_MENU) & 0x8000)                          mods |= MINIMAL_KEY_MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) mods |= MINIMAL_KEY_MOD_SUPER;
    if (GetKeyState(VK_CAPITAL) & 1)                            mods |= MINIMAL_KEY_MOD_CAPS_LOCK;
    if (GetKeyState(VK_NUMLOCK) & 1)                            mods |= MINIMAL_KEY_MOD_NUM_LOCK;
    return mods;
}

static uint8_t MinimalCheckMouseButtons(MinimalWindow* window)
{
    uint8_t i;
    for (i = 0; i <= MINIMAL_MOUSE_BUTTON_LAST; i++)
    {
        if (window->mouse_buttons[i] == MINIMAL_PRESS) break;
    }
    return i;
}

static UINT MinimalGetMouseButton(UINT msg, WPARAM wParam)
{
    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)   return MINIMAL_MOUSE_BUTTON_LEFT;
    if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)   return MINIMAL_MOUSE_BUTTON_RIGHT;
    if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)   return MINIMAL_MOUSE_BUTTON_MIDDLE;
    if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)         return MINIMAL_MOUSE_BUTTON_4;
    else                                                return MINIMAL_MOUSE_BUTTON_5;
}


static void MinimalWindowDispatchEvent(MinimalWindow* window, UINT type, UINT uParam, INT lParam, INT rParam)
{
    if (window->dispatch) window->dispatch(window->event_handler, type, uParam, lParam, rParam);
}

static void MinimalCallChar(MinimalWindow* window, UINT codepoint, UINT mods)
{
    if (window->callbacks.character) window->callbacks.character(window, codepoint, mods);
    MinimalWindowDispatchEvent(window, MINIMAL_EVENT_CHAR, codepoint, 0, mods);
}

static void MinimalCallKey(MinimalWindow* window, UINT key, UINT scancode, UINT action, UINT mods)
{
    if (MinimalKeycodeValid(key)) window->key_state[key].action = (uint8_t)action;
    if (window->callbacks.key) window->callbacks.key(window, key, scancode, action, mods);
    MinimalWindowDispatchEvent(window, MINIMAL_EVENT_KEY, key, action, mods);
}

static void MinimalCallMouseButton(MinimalWindow* window, UINT button, UINT action, UINT mods, INT x, INT y)
{
    if (MinimalMouseButtonValid(button)) window->mouse_buttons[button] = (uint8_t)action;
    if (window->callbacks.m_button) window->callbacks.m_button(window, button, action, mods, x, y);
    MinimalWindowDispatchEvent(window, MINIMAL_EVENT_MOUSE_BUTTON, MINIMAL_MAKE_MOUSE_U(button, action), x, y);
}

static void MinimalCallCursorPos(MinimalWindow* window, UINT x, UINT y)
{
    if (window->callbacks.cursor_pos) window->callbacks.cursor_pos(window, x, y);
    MinimalWindowDispatchEvent(window, MINIMAL_EVENT_MOUSE_MOVED, 0, x, y);
}

static void MinimalCallMouseScroll(MinimalWindow* window, INT h, INT v)
{
    if (window->callbacks.scroll) window->callbacks.scroll(window, h, v);
    MinimalWindowDispatchEvent(window, MINIMAL_EVENT_MOUSE_SCROLLED, 0, h, v);
}

static void MinimalCallWindowSize(MinimalWindow* window, uint32_t w, uint32_t h)
{
    window->width = w;
    window->height = h;

    if (window->callbacks.size) window->callbacks.size(window, w, h);
    MinimalWindowDispatchEvent(window, MINIMAL_EVENT_WINDOW_SIZE, 0, w, h);
}

static void MinimalCallWindowIconify(MinimalWindow* window, MinimalBool iconified)
{
    if (window->callbacks.iconify) window->callbacks.iconify(window, iconified);
    window->iconified = iconified;
}

static void MinimalCallWindowMaximize(MinimalWindow* window, MinimalBool maximized)
{
    if (window->callbacks.maximize) window->callbacks.maximize(window, maximized);
    window->maximized = maximized;
}

static LRESULT MinimalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MinimalWindow* window = GetPropW(hwnd, L"Minimal");
    if (!window) return DefWindowProcW(hwnd, msg, wParam, lParam);
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
    {
        if (window) window->should_close = 1;
        return 0;
    }
    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_UNICHAR:
    {
        MinimalCallChar(window, (UINT)wParam, MinimalGetKeyMods());
        return 0;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        const UINT scancode = (lParam >> 16) & 0x1ff;
        const UINT keycode = MinimalTranslateKey(scancode);
        const UINT action = ((lParam >> 31) & 1) ? MINIMAL_RELEASE : MINIMAL_PRESS;
        const UINT mods = MinimalGetKeyMods();

        MinimalCallKey(window, keycode, scancode, action, mods);
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
        const UINT button = MinimalGetMouseButton(msg, wParam);
        const UINT action = MINIMAL_MSG_BUTTON_DOWN(msg) ? MINIMAL_PRESS : MINIMAL_RELEASE;
        const UINT mods = MinimalGetKeyMods();
        const INT x = MINIMAL_GET_X_LPARAM(lParam);
        const INT y = MINIMAL_GET_Y_LPARAM(lParam);

        if (MinimalCheckMouseButtons(window) > MINIMAL_MOUSE_BUTTON_LAST) SetCapture(hwnd);

        MinimalCallMouseButton(window, button, action, mods, x, y);

        if (MinimalCheckMouseButtons(window) > MINIMAL_MOUSE_BUTTON_LAST) ReleaseCapture();

        return msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP;
    }
    case WM_MOUSEMOVE:
    {
        MinimalCallCursorPos(window, MINIMAL_GET_X_LPARAM(lParam), MINIMAL_GET_Y_LPARAM(lParam));
        return 0;
    }
    case WM_MOUSEWHEEL:
    {
        MinimalCallMouseScroll(window, 0, MINIMAL_GET_SCROLL(wParam));
        return 0;
    }
    case WM_MOUSEHWHEEL:
    {
        MinimalCallMouseScroll(window, MINIMAL_GET_SCROLL(wParam), 0);
        return 0;
    }
    case WM_SIZE:
    {
        const MinimalBool iconified = wParam == SIZE_MINIMIZED;
        const MinimalBool maximized = wParam == SIZE_MAXIMIZED || (window->maximized && wParam != SIZE_RESTORED);

        if (window->iconified != iconified) MinimalCallWindowIconify(window, iconified);
        if (window->maximized != maximized) MinimalCallWindowMaximize(window, maximized);

        MinimalCallWindowSize(window, LOWORD(lParam), HIWORD(lParam));
        return 0;
    }
    default: return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

MinimalBool MinimalRegisterWindowClass()
{
    WNDCLASSEXW wnd_class = { 0 };
    wnd_class.cbSize = sizeof(WNDCLASSEXW);
    wnd_class.hInstance = GetModuleHandleW(NULL);
    wnd_class.lpfnWndProc = MinimalWindowProc;
    wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wnd_class.lpszClassName = MINIMAL_WNDCLASSNAME;
    wnd_class.lpszMenuName = NULL;
    wnd_class.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wnd_class.hIconSm = LoadIconW(NULL, IDI_APPLICATION);
    wnd_class.hCursor = LoadCursorW(NULL, IDC_ARROW);

    if (!RegisterClassExW(&wnd_class))
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "Window", "Failed to register WindowClass");
        return MINIMAL_FAIL;
    }
    return MINIMAL_OK;
}

MinimalBool MinimalUnregisterWindowClass()
{
    if (!UnregisterClassW(L"MinimalWindowClass", GetModuleHandleW(NULL)))
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "Window", "Failed to unregister WindowClass");
        return MINIMAL_FAIL;
    }
    return MINIMAL_OK;
}

static HWND _minimal_helper_hwnd;

MinimalBool MinimalCreateHelperWindow()
{
    HINSTANCE instance = GetModuleHandleW(NULL);
    LPCWSTR class_name = MINIMAL_WNDCLASSNAME;
    DWORD ex_style = WS_EX_OVERLAPPEDWINDOW;
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    _minimal_helper_hwnd = CreateWindowExW(ex_style, class_name, NULL, style, 0, 0, 1, 1, 0, 0, instance, 0);

    if (!_minimal_helper_hwnd)
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "Window", "Failed to create helper window");
        return MINIMAL_FAIL;
    }

    MSG msg;
    while (PeekMessageW(&msg, _minimal_helper_hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return MINIMAL_OK;
}

MinimalBool MinimalDestroyHelperWindow()
{
    if (_minimal_helper_hwnd && !DestroyWindow(_minimal_helper_hwnd))
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "Window", "Failed to destroy helper window");
        return MINIMAL_FAIL;
    }

    _minimal_helper_hwnd = NULL;
    return MINIMAL_OK;
}

HWND MinimalGetHelperWindow()
{
    return _minimal_helper_hwnd;
}

MinimalWindow* MinimalCreateWindow(const char* title, uint32_t w, uint32_t h, const MinimalWindowConfig* config)
{
    MinimalWindow* wnd = MinimalAlloc(sizeof(MinimalWindow));

    wnd->instance = GetModuleHandleW(NULL);
    wnd->width = w;
    wnd->height = h;

    RECT rect = { .right = w, .bottom = h };
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    AdjustWindowRect(&rect, style, 0);

    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    w = rect.right - rect.left;
    h = rect.bottom - rect.top;

    wnd->handle = CreateWindowExW(0, MINIMAL_WNDCLASSNAME, NULL, style, x, y, w, h, 0, 0, wnd->instance, 0);
    wnd->should_close = 0;

    MinimalSetWindowTitle(wnd, title);
    SetPropW(wnd->handle, L"Minimal", wnd);

    memset(wnd->key_state, 0, sizeof(wnd->key_state));
    memset(wnd->mouse_buttons, MINIMAL_RELEASE, sizeof(wnd->mouse_buttons));

    wnd->callbacks.size = NULL;
    wnd->callbacks.close = NULL;
    wnd->callbacks.key = NULL;
    wnd->callbacks.character = NULL;
    wnd->callbacks.m_button = NULL;
    wnd->callbacks.scroll = NULL;
    wnd->callbacks.cursor_pos = NULL;

    if (!MinimalCreateContextWGL(wnd, config->gl_major, config->gl_minor, config->context_flags))
    {
        MinimalDestroyWindow(wnd);
        wnd = NULL;
    }
    return wnd;
}

MinimalBool MinimalDestroyWindow(MinimalWindow* wnd)
{
    MinimalBool status = MinimalDestroyContextWGL(wnd);

    if (wnd->handle && !DestroyWindow(wnd->handle))
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "Window", "Failed to destroy window");
        status = MINIMAL_FAIL;
    }

    MinimalFree(wnd);

    return status;
}

void MinimalPollEvent(MinimalWindow* wnd)
{
    MSG msg;
    while (PeekMessageW(&msg, wnd->handle, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT) MinimalCloseWindow(wnd);

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void MinimalSwapBuffer(MinimalWindow* wnd)                      { SwapBuffers(wnd->device_context); }
void MinimalSetWindowTitle(MinimalWindow* wnd, const char* str) { SetWindowTextA(wnd->handle, str); }

void MinimalSetEventDispatch(MinimalWindow* wnd, void* handler, MinimalDispatchEventCB dispatch)
{
    wnd->event_handler = handler;
    wnd->dispatch = dispatch;
}

uint32_t MinimalGetWindowWidth(const MinimalWindow* wnd)    { return wnd->width; }
uint32_t MinimalGetWindowHeigth(const MinimalWindow* wnd)   { return wnd->height; }

uint8_t MinimalShouldCloseWindow(const MinimalWindow* wnd)  { return wnd->should_close; }
void MinimalCloseWindow(MinimalWindow* wnd)                 { wnd->should_close = 1; }

void MinimalSetSizeCallback(MinimalWindow* wnd, MinimalSizeCB cb)           { wnd->callbacks.size = cb; }
void MinimalSetCloseCallback(MinimalWindow* wnd, MinimalCloseCB cb)         { wnd->callbacks.close = cb; }
void MinimalSetKeyCallback(MinimalWindow* wnd, MinimalKeyCB cb)             { wnd->callbacks.key = cb; }
void MinimalSetCharCallback(MinimalWindow* wnd, MinimalCharCB cb)           { wnd->callbacks.character = cb; }
void MinimalSetMButtonCallback(MinimalWindow* wnd, MinimalMButtonCB cb)     { wnd->callbacks.m_button = cb; }
void MinimalSetScrollCallback(MinimalWindow* wnd, MinimalScrollCB cb)       { wnd->callbacks.scroll = cb; }
void MinimalSetCursorPosCallback(MinimalWindow* wnd, MinimalCursorPosCB cb) { wnd->callbacks.cursor_pos = cb; }


/* --------------------------| input |----------------------------------- */
static uint32_t minimal_key_table[512];

void MinimalCreateKeyTable()
{
    memset(minimal_key_table, MINIMAL_KEY_UNKNOWN, sizeof(minimal_key_table));

    minimal_key_table[0x00B] = MINIMAL_KEY_0;
    minimal_key_table[0x002] = MINIMAL_KEY_1;
    minimal_key_table[0x003] = MINIMAL_KEY_2;
    minimal_key_table[0x004] = MINIMAL_KEY_3;
    minimal_key_table[0x005] = MINIMAL_KEY_4;
    minimal_key_table[0x006] = MINIMAL_KEY_5;
    minimal_key_table[0x007] = MINIMAL_KEY_6;
    minimal_key_table[0x008] = MINIMAL_KEY_7;
    minimal_key_table[0x009] = MINIMAL_KEY_8;
    minimal_key_table[0x00A] = MINIMAL_KEY_9;
    minimal_key_table[0x01E] = MINIMAL_KEY_A;
    minimal_key_table[0x030] = MINIMAL_KEY_B;
    minimal_key_table[0x02E] = MINIMAL_KEY_C;
    minimal_key_table[0x020] = MINIMAL_KEY_D;
    minimal_key_table[0x012] = MINIMAL_KEY_E;
    minimal_key_table[0x021] = MINIMAL_KEY_F;
    minimal_key_table[0x022] = MINIMAL_KEY_G;
    minimal_key_table[0x023] = MINIMAL_KEY_H;
    minimal_key_table[0x017] = MINIMAL_KEY_I;
    minimal_key_table[0x024] = MINIMAL_KEY_J;
    minimal_key_table[0x025] = MINIMAL_KEY_K;
    minimal_key_table[0x026] = MINIMAL_KEY_L;
    minimal_key_table[0x032] = MINIMAL_KEY_M;
    minimal_key_table[0x031] = MINIMAL_KEY_N;
    minimal_key_table[0x018] = MINIMAL_KEY_O;
    minimal_key_table[0x019] = MINIMAL_KEY_P;
    minimal_key_table[0x010] = MINIMAL_KEY_Q;
    minimal_key_table[0x013] = MINIMAL_KEY_R;
    minimal_key_table[0x01F] = MINIMAL_KEY_S;
    minimal_key_table[0x014] = MINIMAL_KEY_T;
    minimal_key_table[0x016] = MINIMAL_KEY_U;
    minimal_key_table[0x02F] = MINIMAL_KEY_V;
    minimal_key_table[0x011] = MINIMAL_KEY_W;
    minimal_key_table[0x02D] = MINIMAL_KEY_X;
    minimal_key_table[0x015] = MINIMAL_KEY_Y;
    minimal_key_table[0x02C] = MINIMAL_KEY_Z;

    minimal_key_table[0x028] = MINIMAL_KEY_APOSTROPHE;
    minimal_key_table[0x02B] = MINIMAL_KEY_BACKSLASH;
    minimal_key_table[0x033] = MINIMAL_KEY_COMMA;
    minimal_key_table[0x00D] = MINIMAL_KEY_EQUAL;
    minimal_key_table[0x029] = MINIMAL_KEY_GRAVE_ACCENT;
    minimal_key_table[0x01A] = MINIMAL_KEY_LEFT_BRACKET;
    minimal_key_table[0x00C] = MINIMAL_KEY_MINUS;
    minimal_key_table[0x034] = MINIMAL_KEY_PERIOD;
    minimal_key_table[0x01B] = MINIMAL_KEY_RIGHT_BRACKET;
    minimal_key_table[0x027] = MINIMAL_KEY_SEMICOLON;
    minimal_key_table[0x035] = MINIMAL_KEY_SLASH;

    minimal_key_table[0x00E] = MINIMAL_KEY_BACKSPACE;
    minimal_key_table[0x153] = MINIMAL_KEY_DELETE;
    minimal_key_table[0x14F] = MINIMAL_KEY_END;
    minimal_key_table[0x01C] = MINIMAL_KEY_ENTER;
    minimal_key_table[0x001] = MINIMAL_KEY_ESCAPE;
    minimal_key_table[0x147] = MINIMAL_KEY_HOME;
    minimal_key_table[0x152] = MINIMAL_KEY_INSERT;
    minimal_key_table[0x151] = MINIMAL_KEY_PAGE_DOWN;
    minimal_key_table[0x149] = MINIMAL_KEY_PAGE_UP;
    minimal_key_table[0x045] = MINIMAL_KEY_PAUSE;
    minimal_key_table[0x146] = MINIMAL_KEY_PAUSE;
    minimal_key_table[0x039] = MINIMAL_KEY_SPACE;
    minimal_key_table[0x00F] = MINIMAL_KEY_TAB;
    minimal_key_table[0x03A] = MINIMAL_KEY_CAPS_LOCK;
    minimal_key_table[0x145] = MINIMAL_KEY_NUM_LOCK;
    minimal_key_table[0x046] = MINIMAL_KEY_SCROLL_LOCK;
    minimal_key_table[0x03B] = MINIMAL_KEY_F1;
    minimal_key_table[0x03C] = MINIMAL_KEY_F2;
    minimal_key_table[0x03D] = MINIMAL_KEY_F3;
    minimal_key_table[0x03E] = MINIMAL_KEY_F4;
    minimal_key_table[0x03F] = MINIMAL_KEY_F5;
    minimal_key_table[0x040] = MINIMAL_KEY_F6;
    minimal_key_table[0x041] = MINIMAL_KEY_F7;
    minimal_key_table[0x042] = MINIMAL_KEY_F8;
    minimal_key_table[0x043] = MINIMAL_KEY_F9;
    minimal_key_table[0x044] = MINIMAL_KEY_F10;
    minimal_key_table[0x057] = MINIMAL_KEY_F11;
    minimal_key_table[0x058] = MINIMAL_KEY_F12;
    minimal_key_table[0x038] = MINIMAL_KEY_LEFT_ALT;
    minimal_key_table[0x01D] = MINIMAL_KEY_LEFT_CONTROL;
    minimal_key_table[0x02A] = MINIMAL_KEY_LEFT_SHIFT;
    minimal_key_table[0x15B] = MINIMAL_KEY_LEFT_SUPER;
    minimal_key_table[0x137] = MINIMAL_KEY_PRINT_SCREEN;
    minimal_key_table[0x138] = MINIMAL_KEY_RIGHT_ALT;
    minimal_key_table[0x11D] = MINIMAL_KEY_RIGHT_CONTROL;
    minimal_key_table[0x036] = MINIMAL_KEY_RIGHT_SHIFT;
    minimal_key_table[0x15C] = MINIMAL_KEY_RIGHT_SUPER;
    minimal_key_table[0x150] = MINIMAL_KEY_DOWN;
    minimal_key_table[0x14B] = MINIMAL_KEY_LEFT;
    minimal_key_table[0x14D] = MINIMAL_KEY_RIGHT;
    minimal_key_table[0x148] = MINIMAL_KEY_UP;

    minimal_key_table[0x052] = MINIMAL_KEY_NP_0;
    minimal_key_table[0x04F] = MINIMAL_KEY_NP_1;
    minimal_key_table[0x050] = MINIMAL_KEY_NP_2;
    minimal_key_table[0x051] = MINIMAL_KEY_NP_3;
    minimal_key_table[0x04B] = MINIMAL_KEY_NP_4;
    minimal_key_table[0x04C] = MINIMAL_KEY_NP_5;
    minimal_key_table[0x04D] = MINIMAL_KEY_NP_6;
    minimal_key_table[0x047] = MINIMAL_KEY_NP_7;
    minimal_key_table[0x048] = MINIMAL_KEY_NP_8;
    minimal_key_table[0x049] = MINIMAL_KEY_NP_9;
    minimal_key_table[0x04E] = MINIMAL_KEY_NP_ADD;
    minimal_key_table[0x053] = MINIMAL_KEY_NP_DECIMAL;
    minimal_key_table[0x135] = MINIMAL_KEY_NP_DIVIDE;
    minimal_key_table[0x11C] = MINIMAL_KEY_NP_ENTER;
    minimal_key_table[0x059] = MINIMAL_KEY_NP_EQUAL;
    minimal_key_table[0x037] = MINIMAL_KEY_NP_MULTIPLY;
    minimal_key_table[0x04A] = MINIMAL_KEY_NP_SUBTRACT;
}

uint32_t MinimalTranslateKey(uint32_t scancode) { return minimal_key_table[scancode]; }

void MinimalWindowUpdateKeyStates(MinimalWindow* window)
{
    for (uint32_t key = MINIMAL_KEY_ESCAPE; key <= MINIMAL_KEY_LAST; ++key)
    {
        window->key_state[key].previous = window->key_state[key].current;
        window->key_state[key].current = window->key_state[key].action;
    }
}

const MinimalInputState* MinimalWindowGetKeyState(const MinimalWindow* window, uint32_t keycode)
{
    return (window && MinimalKeycodeValid(keycode)) ? &window->key_state[keycode] : NULL;
}

int8_t MinimalWindowGetMouseButtonState(const MinimalWindow* window, uint32_t button)
{
    return (window && MinimalMouseButtonValid(button)) ? window->mouse_buttons[button] : -1;
}

void MinimalWindowGetCursorPos(const MinimalWindow* window, float* x, float* y)
{
    POINT pos = { 0 };
    if (GetCursorPos(&pos)) ScreenToClient(window->handle, &pos);

    if (x) *x = (float)pos.x;
    if (y) *y = (float)pos.y;
}

/* --------------------------| helper |---------------------------------- */
void MinimalWindowSetContext(MinimalWindow* window, HDC dc, HGLRC rc)
{
    window->device_context = dc;
    window->render_context = rc;
}

HWND  MinimalWindowGetHandle(const MinimalWindow* window)        { return window->handle; }
HDC   MinimalWindowGetDeviceContext(const MinimalWindow* window) { return window->device_context; }
HGLRC MinimalWindowGetRenderContext(const MinimalWindow* window) { return window->render_context; }
