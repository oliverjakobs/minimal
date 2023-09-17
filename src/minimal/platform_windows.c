#include "platform.h"

#ifdef MINIMAL_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#define MINIMAL_WNDCLASSNAME L"MINIMALWNDCLASS"

/* Accepted as an attribute name in <*attribList> */
#define WGL_CONTEXT_MAJOR_VERSION_ARB               0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB               0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB                 0x2093
#define WGL_CONTEXT_FLAGS_ARB                       0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB                0x9126

/* Accepted as bits in the attribute value for WGL_CONTEXT_FLAGS in <*attribList> */
#define WGL_CONTEXT_DEBUG_BIT_ARB                   0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB      0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB            0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB   0x00000002

// See https://www.opengl.org/registry/specs/ARB/wgl_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                      0x2001
#define WGL_ACCELERATION_ARB                        0x2003
#define WGL_SUPPORT_OPENGL_ARB                      0x2010
#define WGL_DOUBLE_BUFFER_ARB                       0x2011
#define WGL_PIXEL_TYPE_ARB                          0x2013
#define WGL_COLOR_BITS_ARB                          0x2014
#define WGL_DEPTH_BITS_ARB                          0x2022
#define WGL_STENCIL_BITS_ARB                        0x2023

#define WGL_FULL_ACCELERATION_ARB                   0x2027
#define WGL_TYPE_RGBA_ARB                           0x202B

#include "event.h"

// WGL extension pointer typedefs
typedef BOOL(WINAPI* wglSwapIntervalEXT_T)(int);
typedef BOOL(WINAPI* wglChoosePixelFormatARB_T)(HDC, const int*, const float*, UINT, int*, UINT*);
typedef HGLRC(WINAPI* wglCreateContextAttribsARB_T)(HDC, HGLRC, const int*);

#define MINIMAL_GET_X_LPARAM(lp)    ((int32_t)(int16_t)LOWORD(lp))
#define MINIMAL_GET_Y_LPARAM(lp)    ((int32_t)(int16_t)HIWORD(lp))

#define MINIMAL_GET_SCROLL(wp)      ((int32_t)((int16_t)HIWORD(wp) / (float)WHEEL_DELTA))

static uint64_t _minimal_timer_frequency = 0;
static uint64_t _minimal_timer_offset = 0;

static wglCreateContextAttribsARB_T _wglCreateContextAttrARB = NULL;
static wglSwapIntervalEXT_T         _wglSwapIntervalEXT = NULL;
static wglChoosePixelFormatARB_T    _wglChoosePixelFormatARB = NULL;

static HMODULE gl_module = NULL;

static LRESULT minimalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void minimalCreateKeyTable();

static uint8_t minimalWGLInit()
{
    // load opengl library
    gl_module = LoadLibraryA("opengl32.dll");
    if (!gl_module)
    {
        MINIMAL_ERROR("[WGL] Failed to load opengl32.dll");
        return MINIMAL_FAIL;
    }

    // create helper window
    HINSTANCE instance = GetModuleHandleW(NULL);
    LPCWSTR class_name = MINIMAL_WNDCLASSNAME;
    DWORD ex_style = WS_EX_OVERLAPPEDWINDOW;
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    HWND helper_wnd = CreateWindowExW(ex_style, class_name, NULL, style, 0, 0, 1, 1, 0, 0, instance, 0);

    if (!helper_wnd)
    {
        MINIMAL_ERROR("[WGL] Failed to create helper window");
        return MINIMAL_FAIL;
    }

    MSG msg;
    while (PeekMessageW(&msg, helper_wnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    HDC dc = GetDC(helper_wnd);

    PIXELFORMATDESCRIPTOR pfd = {
        .nSize = sizeof(pfd),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 24,
    };

    if (!SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd))
    {
        MINIMAL_ERROR("[WGL] Failed to set pixel format for dummy context");
        return MINIMAL_FAIL;
    }

    HGLRC rc = wglCreateContext(dc);
    if (!rc)
    {
        MINIMAL_ERROR("[WGL] Failed to create dummy context");
        return MINIMAL_FAIL;
    }

    HDC   pdc = wglGetCurrentDC();
    HGLRC prc = wglGetCurrentContext();

    if (!wglMakeCurrent(dc, rc))
    {
        MINIMAL_ERROR("[WGL] Failed to make dummy context current");
        wglMakeCurrent(pdc, prc);
        wglDeleteContext(rc);
        return MINIMAL_FAIL;
    }

    _wglCreateContextAttrARB = (wglCreateContextAttribsARB_T)wglGetProcAddress("wglCreateContextAttribsARB");
    _wglSwapIntervalEXT =      (wglSwapIntervalEXT_T)wglGetProcAddress("wglSwapIntervalEXT");
    _wglChoosePixelFormatARB = (wglChoosePixelFormatARB_T)wglGetProcAddress("wglChoosePixelFormatARB");

    wglMakeCurrent(pdc, prc);
    wglDeleteContext(rc);

    // destroy helper window
    if (!DestroyWindow(helper_wnd))
    {
        MINIMAL_ERROR("[WGL] Failed to destroy helper window");
        return MINIMAL_FAIL;
    }

    return MINIMAL_OK;
}

static void minimalWGLTerminate()
{
    if (gl_module)
        FreeLibrary(gl_module);
}

uint8_t minimalPlatformInit()
{
    // register window class
    WNDCLASSEXW wnd_class = {
        .cbSize         = sizeof(WNDCLASSEXW),
        .hInstance      = GetModuleHandleW(NULL),
        .lpfnWndProc    = minimalWindowProc,
        .style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpszClassName  = MINIMAL_WNDCLASSNAME,
        .lpszMenuName   = NULL,
        .hIcon          = LoadIconW(NULL, IDI_APPLICATION),
        .hIconSm        = LoadIconW(NULL, IDI_APPLICATION),
        .hCursor        = LoadCursorW(NULL, IDC_ARROW)
    };

    if (!RegisterClassExW(&wnd_class))
    {
        MINIMAL_ERROR("[Platform] Failed to register WindowClass");
        return MINIMAL_FAIL;
    }

    // init wgl
    if (!minimalWGLInit())
    {
        MINIMAL_ERROR("[Platform] Failed to initialize WGL");
        return MINIMAL_FAIL;
    }

    // init time
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)&_minimal_timer_frequency))
    {
        MINIMAL_ERROR("[Platform] High-resolution performance counter is not supported");
        return MINIMAL_FAIL;
    }

    QueryPerformanceCounter((LARGE_INTEGER*)&_minimal_timer_offset);

    // init input
    minimalCreateKeyTable();

    return MINIMAL_OK;
}

uint8_t minimalPlatformTerminate()
{
    minimalWGLTerminate();

    // unregister window class
    if (!UnregisterClassW(MINIMAL_WNDCLASSNAME, GetModuleHandleW(NULL)))
    {
        MINIMAL_ERROR("[Platform] Failed to unregister WindowClass");
        return MINIMAL_FAIL;
    }
    return MINIMAL_OK;
}

struct MinimalWindow
{
    HINSTANCE   instance;
    HWND        handle;
    HDC         device_context;
    HGLRC       render_context;

    uint8_t should_close : 1;
    uint8_t maximized : 1;
    uint8_t minimized : 1;

    uint8_t key_state[MINIMAL_KEY_LAST + 1];
    uint8_t mouse_buttons[MINIMAL_MOUSE_BUTTON_LAST + 1];

    void* event_handler;
};

static uint8_t minimalCreateRenderContext(MinimalWindow* window, const MinimalWndConfig* config)
{
    HDC dc = GetDC(window->handle);
    if (!dc)
    {
        MINIMAL_ERROR("[Platform] Failed to retrieve device context handle");
        return MINIMAL_FAIL;
    }

    int pf_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     1,
        WGL_SUPPORT_OPENGL_ARB,     1,
        WGL_DOUBLE_BUFFER_ARB,      1,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        0
    };

    int pixel_format;
    UINT num_formats;
    _wglChoosePixelFormatARB(dc, pf_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats)
    {
        MINIMAL_ERROR("[WGL] Could not find a suitable pixel format");
        return MINIMAL_FAIL;
    }

    PIXELFORMATDESCRIPTOR pfd;
    if (!DescribePixelFormat(dc, pixel_format, sizeof(pfd), &pfd))
    {
        MINIMAL_ERROR("[WGL] Failed to retrieve PFD");
        return MINIMAL_FAIL;
    }

    if (!SetPixelFormat(dc, pixel_format, &pfd))
    {
        MINIMAL_ERROR("[WGL] Failed to set pixel format");
        return MINIMAL_FAIL;
    }

    int gl_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,  config->gl_major,
        WGL_CONTEXT_MINOR_VERSION_ARB,  config->gl_minor,
        WGL_CONTEXT_PROFILE_MASK_ARB,   config->profile,
        WGL_CONTEXT_FLAGS_ARB,          config->flags,
        0,
    };

    HGLRC rc = _wglCreateContextAttrARB(dc, 0, gl_attribs);
    if (!rc)
    {
        MINIMAL_ERROR("[WGL] Failed to create render context");
        return MINIMAL_FAIL;
    }

    if (!wglMakeCurrent(dc, rc))
    {
        MINIMAL_ERROR("[WGL] Failed to make render context current");
        return MINIMAL_FAIL;
    }

    window->device_context = dc;
    window->render_context = rc;

    return MINIMAL_OK;
}

MinimalWindow* minimalCreateWindow(const char* title, uint32_t w, uint32_t h, const MinimalWndConfig* config)
{
    MinimalWindow* window = malloc(sizeof(MinimalWindow));
    if (!window) return NULL;

    window->should_close = 0;
    window->maximized = 0;
    window->minimized = 0;

    window->event_handler = NULL;

    memset(window->key_state,     MINIMAL_RELEASE, sizeof(window->key_state));
    memset(window->mouse_buttons, MINIMAL_RELEASE, sizeof(window->mouse_buttons));

    // create window
    window->instance = GetModuleHandleW(NULL);

    RECT rect = { .right = w, .bottom = h };
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    AdjustWindowRect(&rect, style, 0);

    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    w = rect.right - rect.left;
    h = rect.bottom - rect.top;

    window->handle = CreateWindowExW(0, MINIMAL_WNDCLASSNAME, NULL, style, x, y, w, h, 0, 0, window->instance, 0);
    if (!window->handle)
    {
        MINIMAL_ERROR("[Platform] Failed to create window");
        minimalDestroyWindow(window);
        return NULL;
    }

    minimalSetWindowTitle(window, title);

    if (!minimalCreateRenderContext(window, config))
    {
        minimalDestroyWindow(window);
        return NULL;
    }

    return window;
}

void minimalDestroyWindow(MinimalWindow* window)
{
    // destroy render context
    if (window->render_context)
    {
        if (!wglMakeCurrent(NULL, NULL))
        {
            MINIMAL_ERROR("[WGL] Failed to realease render context");
        }

        if (!wglDeleteContext(window->render_context))
        {
            MINIMAL_ERROR("[WGL] Failed to delete render context");
        }
    }

    // destroy device context
    if (window->device_context && !ReleaseDC(window->handle, window->device_context))
    {
        MINIMAL_ERROR("[Platform] Failed to release device context");
    }

    // destroy window
    if (window->handle && !DestroyWindow(window->handle))
    {
        MINIMAL_ERROR("[Platform] Failed to destroy window");
    }

    free(window);
}

void* minimalGetGLProcAddress(const char* name)
{
    void* p = (void*)wglGetProcAddress(name);
    if (p) return p;

    return (void*)GetProcAddress(gl_module, name);;
}


void minimalSetWindowEventHandler(MinimalWindow* window, void* handler)
{
    window->event_handler = handler;
}

void* minimalGetWindowEventHandler(MinimalWindow* window)
{
    return window->event_handler;
}

static MinimalWindow* _current_context;

void minimalMakeContextCurrent(MinimalWindow* context) { _current_context = context; }
MinimalWindow* minimalGetCurrentContext()              { return _current_context; }

void minimalPollWindowEvents(MinimalWindow* context)
{
    MSG msg;
    while (PeekMessageW(&msg, context->handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void minimalSwapBuffers(MinimalWindow* context)
{
    SwapBuffers(context->device_context);
}

void minimalSetWindowTitle(MinimalWindow* context, const char* title)
{
    SetWindowTextA(context->handle, title);
}

void minimalSwapInterval(uint8_t interval)
{
    if (_wglSwapIntervalEXT) _wglSwapIntervalEXT(interval);
}

uint8_t minimalShouldClose(const MinimalWindow* context) { return context->should_close; }
void    minimalCloseWindow(MinimalWindow* context)       { context->should_close = 1; }

void    minimalWindowMinimize(MinimalWindow* context, uint8_t minimized) { context->minimized = minimized; }
void    minimalWindowMaximize(MinimalWindow* context, uint8_t maximized) { context->maximized = maximized; }
uint8_t minimalWindowIsMinimized(const MinimalWindow* context) { return context->minimized; }
uint8_t minimalWindowIsMaximized(const MinimalWindow* context) { return context->maximized; }

void minimalGetContentSize(const MinimalWindow* context, int32_t* w, int32_t* h)
{
    RECT rect;
    if (GetClientRect(context->handle, &rect))
    {
        if (w) *w = rect.right - rect.left;
        if (h) *h = rect.bottom - rect.top;
    }
}

void minimalGetWindowSize(const MinimalWindow* context, int32_t* w, int32_t* h)
{
    RECT rect;
    if (GetWindowRect(context->handle, &rect))
    {
        if (w) *w = rect.right - rect.left;
        if (h) *h = rect.bottom - rect.top;
    }
}

int8_t minimalGetKeyState(const MinimalWindow* context, uint32_t keycode)
{
    return minimalKeycodeValid(keycode) ? context->key_state[keycode] : -1;
}

int8_t minimalGetMouseButtonState(const MinimalWindow* context, uint32_t button)
{
    return minimalMouseButtonValid(button) ? context->mouse_buttons[button] : -1;
}

void minimalGetCursorPos(const MinimalWindow* context, float* x, float* y)
{
    POINT pos = { 0 };
    if (GetCursorPos(&pos)) ScreenToClient(context->handle, &pos);

    if (x) *x = (float)pos.x;
    if (y) *y = (float)pos.y;
}

double minimalGetTime()
{
    uint64_t value;
    QueryPerformanceCounter((LARGE_INTEGER*)&value);

    return (double)(value - _minimal_timer_offset) / _minimal_timer_frequency;
}


static uint32_t minimalGetKeyMods()
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

static uint32_t minimalGetMouseButton(UINT msg, WPARAM wParam)
{
    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)   return MINIMAL_MOUSE_BUTTON_LEFT;
    if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)   return MINIMAL_MOUSE_BUTTON_RIGHT;
    if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)   return MINIMAL_MOUSE_BUTTON_MIDDLE;
    if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)         return MINIMAL_MOUSE_BUTTON_4;
    else                                                return MINIMAL_MOUSE_BUTTON_5;
}

static uint8_t minimalCheckMouseButtons(MinimalWindow* context)
{
    for (uint8_t i = 0; i <= MINIMAL_MOUSE_BUTTON_LAST; i++)
        if (context->mouse_buttons[i] == MINIMAL_PRESS) return i;

    return MINIMAL_MOUSE_BUTTON_LAST;
}

static uint32_t minimal_key_table[512];

static void minimalCreateKeyTable()
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

static LRESULT minimalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!_current_context) return DefWindowProcW(hwnd, msg, wParam, lParam);

    MinimalApp* event_handler = minimalGetWindowEventHandler(_current_context);

    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_QUIT:
    case WM_CLOSE:
    {
        minimalCloseWindow(_current_context);
        return 0;
    }
    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_UNICHAR:
    {
        uint32_t codepoint = (uint32_t)wParam;
        uint32_t mods = minimalGetKeyMods();
        if (codepoint > 31)
            minimalDispatchEvent(event_handler, MINIMAL_EVENT_CHAR, codepoint, 0, mods);
        return 0;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        uint32_t scancode = (lParam >> 16) & 0x1ff;
        uint32_t keycode = minimal_key_table[scancode];
        uint32_t action = ((lParam >> 31) & 1) ? MINIMAL_RELEASE : MINIMAL_PRESS;
        uint32_t mods = minimalGetKeyMods();

        if (minimalKeycodeValid(keycode)) _current_context->key_state[keycode] = (uint8_t)action;

        minimalDispatchEvent(event_handler, MINIMAL_EVENT_KEY, keycode, action, mods);
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
        uint32_t action = msg == WM_LBUTTONDOWN
                        || msg == WM_RBUTTONDOWN
                        || msg == WM_MBUTTONDOWN
                        || msg == WM_XBUTTONDOWN;

        uint32_t button = minimalGetMouseButton(msg, wParam);
        uint32_t mods = minimalGetKeyMods();
        int32_t x = MINIMAL_GET_X_LPARAM(lParam);
        int32_t y = MINIMAL_GET_Y_LPARAM(lParam);

        if (minimalCheckMouseButtons(_current_context) > MINIMAL_MOUSE_BUTTON_LAST) SetCapture(hwnd);

        if (minimalMouseButtonValid(button)) _current_context->mouse_buttons[button] = (uint8_t)action;

        minimalDispatchEvent(event_handler, MINIMAL_EVENT_MOUSE_BUTTON, (button << 16) + action, x, y);

        if (minimalCheckMouseButtons(_current_context) > MINIMAL_MOUSE_BUTTON_LAST) ReleaseCapture();

        return msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP;
    }
    case WM_MOUSEMOVE:
    {
        int32_t x = MINIMAL_GET_X_LPARAM(lParam);
        int32_t y = MINIMAL_GET_Y_LPARAM(lParam);

        minimalDispatchEvent(event_handler, MINIMAL_EVENT_MOUSE_MOVED, 0, x, y);
        return 0;
    }
    case WM_MOUSEWHEEL:
    {
        int32_t scroll = MINIMAL_GET_SCROLL(wParam);

        minimalDispatchEvent(event_handler, MINIMAL_EVENT_MOUSE_SCROLLED, 0, 0, scroll);
        return 0;
    }
    case WM_MOUSEHWHEEL:
    {
        int32_t scroll = MINIMAL_GET_SCROLL(wParam);

        minimalDispatchEvent(event_handler, MINIMAL_EVENT_MOUSE_SCROLLED, 0, scroll, 0);
        return 0;
    }
    case WM_SIZE:
    {
        uint8_t minimized = wParam == SIZE_MINIMIZED;
        uint8_t maximized = wParam == SIZE_MAXIMIZED || (_current_context->maximized && wParam != SIZE_RESTORED);

        if (_current_context->minimized != minimized)
        {
            _current_context->minimized = minimized;
            minimalDispatchEvent(event_handler, MINIMAL_EVENT_WINDOW_MINIMIZE, minimized, 0, 0);
        }

        if (_current_context->maximized != maximized)
        {
            _current_context->maximized = maximized;
            minimalDispatchEvent(event_handler, MINIMAL_EVENT_WINDOW_MAXIMIZE, maximized, 0, 0);
        }

        int32_t width  = LOWORD(lParam);
        int32_t height = HIWORD(lParam);

        minimalDispatchEvent(event_handler, MINIMAL_EVENT_WINDOW_SIZE, 0, width, height);
        return 0;
    }
    default: return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

#endif // MINIMAL_PLATFORM_WINDOWS