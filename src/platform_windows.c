#include "minimal.h"

#ifdef MINIMAL_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define MINIMAL_WNDCLASSNAME L"MINIMALWNDCLASS"

static uint64_t _minimalTimerFrequency = 0;
static uint64_t _minimalTimerOffset = 0;

static LRESULT minimalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifndef MINIMAL_NO_CONTEXT

static uint8_t minimalWGLInit();
static void minimalWGLTerminate();
static HGLRC minimalCreateRenderContext(HDC dc);

#endif

uint8_t minimalPlatformInit()
{
    // register window class
    WNDCLASSEXW wndClass = {
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

    if (!RegisterClassExW(&wndClass))
    {
        MINIMAL_ERROR("[Platform] Failed to register WindowClass");
        return FALSE;
    }

    // init time
    if (!QueryPerformanceFrequency((LARGE_INTEGER*)&_minimalTimerFrequency))
    {
        MINIMAL_ERROR("[Platform] High-resolution performance counter is not supported");
        return MINIMAL_FAIL;
    }

    QueryPerformanceCounter((LARGE_INTEGER*)&_minimalTimerOffset);

#ifndef MINIMAL_NO_CONTEXT
    // init wgl
    if (!minimalWGLInit())
    {
        MINIMAL_ERROR("[Platform] Failed to initialize WGL");
        return MINIMAL_FAIL;
    }
#endif

    return MINIMAL_OK;
}

uint8_t minimalPlatformTerminate()
{
#ifndef MINIMAL_NO_CONTEXT
    minimalWGLTerminate();
#endif

    // unregister window class
    if (!UnregisterClassW(MINIMAL_WNDCLASSNAME, GetModuleHandleW(NULL)))
    {
        MINIMAL_ERROR("[Platform] Failed to unregister WindowClass");
        return MINIMAL_FAIL;
    }
    return MINIMAL_OK;
}

static int32_t windowHints[MINIMAL_HINT_MAX_ENUM];

void minimalSetWindowHint(MinimalWindowHint name, int32_t value)
{
    if (name >= 0 && name < MINIMAL_HINT_MAX_ENUM)
        windowHints[name] = value;
}

struct MinimalWindow
{
    HWND handle;

#ifndef MINIMAL_NO_CONTEXT
    HDC         deviceContext;
    HGLRC       renderContext;
#endif

    uint8_t shouldClose;
};

MinimalWindow* minimalCreateWindow(const char* title, int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    MinimalWindow* window = malloc(sizeof(MinimalWindow));
    if (!window) return NULL;

    // create window
    HINSTANCE instance = GetModuleHandleW(NULL);

    RECT rect = { 0 };
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    DWORD styleEx = WS_EX_APPWINDOW;
    AdjustWindowRectEx(&rect, style, 0, styleEx);

    x += rect.left;
    y += rect.top;
    w += rect.right - rect.left;
    h += rect.bottom - rect.top;

    window->handle = CreateWindowExW(styleEx, MINIMAL_WNDCLASSNAME, NULL, style, x, y, w, h, 0, 0, instance, 0);
    if (!window->handle)
    {
        MINIMAL_ERROR("[Platform] Failed to create window");
        minimalDestroyWindow(window);
        return NULL;
    }

    minimalSetWindowTitle(window, title);

    window->shouldClose = 0;

#ifndef MINIMAL_NO_CONTEXT

    window->deviceContext = GetDC(window->handle);
    window->renderContext = minimalCreateRenderContext(window->deviceContext);
    if (!window->renderContext)
    {
        minimalDestroyWindow(window);
        return NULL;
    }

#endif

    return window;
}

void minimalDestroyWindow(MinimalWindow* window)
{
#ifndef MINIMAL_NO_CONTEXT

    // destroy render context
    if (window->renderContext)
    {
        if (!wglMakeCurrent(NULL, NULL))
        {
            MINIMAL_ERROR("[WGL] Failed to realease render context");
        }

        if (!wglDeleteContext(window->renderContext))
        {
            MINIMAL_ERROR("[WGL] Failed to delete render context");
        }
    }

    // destroy device context
    if (window->deviceContext && !ReleaseDC(window->handle, window->deviceContext))
    {
        MINIMAL_ERROR("[Platform] Failed to release device context");
    }

#endif

    // destroy window
    if (window->handle && !DestroyWindow(window->handle))
    {
        MINIMAL_ERROR("[Platform] Failed to destroy window");
    }

    free(window);
}

void minimalPollWindowEvents(MinimalWindow* context)
{
    MSG msg;
    while (PeekMessageW(&msg, context->handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void minimalSetWindowTitle(MinimalWindow* context, const char* title)
{
    SetWindowTextA(context->handle, title);
}


uint8_t minimalShouldClose(const MinimalWindow* context) { return context->shouldClose; }
void    minimalClose(MinimalWindow* context)             { context->shouldClose = 1; }

void minimalMaximize(MinimalWindow* window) { ShowWindow(window->handle, SW_SHOWMAXIMIZED); }
void minimalMinimize(MinimalWindow* window) { ShowWindow(window->handle, SW_MINIMIZE); }

double minimalGetTime()
{
    uint64_t value;
    QueryPerformanceCounter((LARGE_INTEGER*)&value);

    return (double)(value - _minimalTimerOffset) / _minimalTimerFrequency;
}

void minimalGetFramebufferSize(const MinimalWindow* context, int32_t* w, int32_t* h)
{
    RECT rect;
    if (GetClientRect(context->handle, &rect))
    {
        if (w) *w = rect.right - rect.left;
        if (h) *h = rect.bottom - rect.top;
    }
}

void minimalGetWindowContentScale(const MinimalWindow* context, float* xscale, float* yscale)
{
    const HDC dc = GetDC(NULL);
    UINT xdpi = GetDeviceCaps(dc, LOGPIXELSX);
    UINT ydpi = GetDeviceCaps(dc, LOGPIXELSY);
    ReleaseDC(NULL, dc);

    if (xscale) *xscale = xdpi / (float)USER_DEFAULT_SCREEN_DPI;
    if (yscale) *yscale = ydpi / (float)USER_DEFAULT_SCREEN_DPI;
}

void* minimalGetNativeWindowHandle(const MinimalWindow* window)
{
    return window->handle;
}


static uint32_t minimalGetKeyMods()
{
    uint32_t mods = 0;
    if (GetKeyState(VK_SHIFT) & 0x8000)                         mods |= MINIMAL_KEY_MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) & 0x8000)                       mods |= MINIMAL_KEY_MOD_CONTROL;
    if (GetKeyState(VK_MENU) & 0x8000)                          mods |= MINIMAL_KEY_MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) mods |= MINIMAL_KEY_MOD_COMMAND;
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

#define MINIMAL_GET_X_LPARAM(lp)    ((int32_t)(int16_t)LOWORD(lp))
#define MINIMAL_GET_Y_LPARAM(lp)    ((int32_t)(int16_t)HIWORD(lp))

#define MINIMAL_GET_SCROLL(wp)      ((int32_t)((int16_t)HIWORD(wp) / (float)WHEEL_DELTA))

static LRESULT minimalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MinimalWindow* context = minimalGetCurrentContext();

    if (!context) return DefWindowProcW(hwnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_QUIT:
    case WM_CLOSE:
        minimalClose(context);
        return 0;
    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_UNICHAR:
    {
        uint32_t codepoint = (uint32_t)wParam;
        uint32_t mods = minimalGetKeyMods();

        if (codepoint > 31)
            minimalDispatchEvent(MINIMAL_EVENT_CHAR, codepoint, 0, mods);

        return 0;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        uint8_t action = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
        uint32_t keycode = (uint16_t)wParam;
        uint32_t mods = minimalGetKeyMods();

        if (minimalProcessKey(keycode, action))
            minimalDispatchEvent(MINIMAL_EVENT_KEY, keycode, action, mods);

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

        uint16_t button = minimalGetMouseButton(msg, wParam);
        int32_t x = MINIMAL_GET_X_LPARAM(lParam);
        int32_t y = MINIMAL_GET_Y_LPARAM(lParam);

        if (minimalProcessMouseButton(button, action))
            minimalDispatchEvent(MINIMAL_EVENT_MOUSE_BUTTON, (button << 16) + action, x, y);

        return msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP;
    }
    case WM_MOUSEMOVE:
    {
        int32_t x = MINIMAL_GET_X_LPARAM(lParam);
        int32_t y = MINIMAL_GET_Y_LPARAM(lParam);

        if (minimalProcessMouseMove((float)x, (float)y))
            minimalDispatchEvent(MINIMAL_EVENT_MOUSE_MOVED, 0, x, y);
        return 0;
    }
    case WM_MOUSEWHEEL:
    {
        int32_t scroll = MINIMAL_GET_SCROLL(wParam);

        minimalDispatchEvent(MINIMAL_EVENT_MOUSE_SCROLLED, 0, 0, scroll);
        return 0;
    }
    case WM_MOUSEHWHEEL:
    {
        int32_t scroll = MINIMAL_GET_SCROLL(wParam);

        minimalDispatchEvent(MINIMAL_EVENT_MOUSE_SCROLLED, 0, scroll, 0);
        return 0;
    }
    case WM_SIZE:
    {
        /*
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
        */

        int32_t width  = LOWORD(lParam);
        int32_t height = HIWORD(lParam);

        minimalDispatchEvent(MINIMAL_EVENT_WINDOW_SIZE, 0, width, height);
        return 0;
    }
    default: return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

/* --------------------------| wgl |------------------------------------- */
#ifndef MINIMAL_NO_CONTEXT

/* Accepted as an attribute name in <*attribList> */
#define WGL_CONTEXT_MAJOR_VERSION_ARB               0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB               0x2092
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


// WGL extension pointer typedefs
typedef BOOL(WINAPI* wglSwapIntervalEXT_T)(int);
typedef BOOL(WINAPI* wglChoosePixelFormatARB_T)(HDC, const int*, const float*, UINT, int*, UINT*);
typedef HGLRC(WINAPI* wglCreateContextAttribsARB_T)(HDC, HGLRC, const int*);

static wglCreateContextAttribsARB_T _wglCreateContextAttrARB = NULL;
static wglSwapIntervalEXT_T         _wglSwapIntervalEXT = NULL;
static wglChoosePixelFormatARB_T    _wglChoosePixelFormatARB = NULL;

static HMODULE glModule = NULL;

uint8_t minimalWGLInit()
{
    // load opengl library
    glModule = LoadLibraryA("opengl32.dll");
    if (!glModule)
    {
        MINIMAL_ERROR("[WGL] Failed to load opengl32.dll");
        return MINIMAL_FAIL;
    }

    // create helper window
    HINSTANCE instance = GetModuleHandleW(NULL);
    LPCWSTR className = MINIMAL_WNDCLASSNAME;
    DWORD exStyle = WS_EX_OVERLAPPEDWINDOW;
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    HWND helperWnd = CreateWindowExW(exStyle, className, NULL, style, 0, 0, 1, 1, 0, 0, instance, 0);

    if (!helperWnd)
    {
        MINIMAL_ERROR("[WGL] Failed to create helper window");
        return MINIMAL_FAIL;
    }

    MSG msg;
    while (PeekMessageW(&msg, helperWnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    HDC dc = GetDC(helperWnd);

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

    _wglCreateContextAttrARB =  (wglCreateContextAttribsARB_T)wglGetProcAddress("wglCreateContextAttribsARB");
    _wglSwapIntervalEXT =       (wglSwapIntervalEXT_T)wglGetProcAddress("wglSwapIntervalEXT");
    _wglChoosePixelFormatARB =  (wglChoosePixelFormatARB_T)wglGetProcAddress("wglChoosePixelFormatARB");

    wglMakeCurrent(pdc, prc);
    wglDeleteContext(rc);

    // destroy helper window
    if (!DestroyWindow(helperWnd))
    {
        MINIMAL_ERROR("[WGL] Failed to destroy helper window");
        return MINIMAL_FAIL;
    }

    return MINIMAL_OK;
}

void minimalWGLTerminate()
{
    if (glModule)
        FreeLibrary(glModule);
}

HGLRC minimalCreateRenderContext(HDC dc)
{
    int pfAttribs[] = {
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

    int format;
    UINT formatCount;
    _wglChoosePixelFormatARB(dc, pfAttribs, 0, 1, &format, &formatCount);
    if (!formatCount)
    {
        MINIMAL_ERROR("[WGL] Could not find a suitable pixel format");
        return NULL;
    }

    PIXELFORMATDESCRIPTOR pfd;
    if (!DescribePixelFormat(dc, format, sizeof(pfd), &pfd))
    {
        MINIMAL_ERROR("[WGL] Failed to retrieve PFD");
        return NULL;
    }

    if (!SetPixelFormat(dc, format, &pfd))
    {
        MINIMAL_ERROR("[WGL] Failed to set pixel format");
        return NULL;
    }

    int glAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,  windowHints[MINIMAL_HINT_CONTEXT_MAJOR_VERSION],
        WGL_CONTEXT_MINOR_VERSION_ARB,  windowHints[MINIMAL_HINT_CONTEXT_MINOR_VERSION],
        WGL_CONTEXT_PROFILE_MASK_ARB,   WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef _DEBUG
        WGL_CONTEXT_FLAGS_ARB,          WGL_CONTEXT_DEBUG_BIT_ARB,
#endif // _DEBUG
        0,
    };

    HGLRC rc = _wglCreateContextAttrARB(dc, 0, glAttribs);
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

    return rc;
}

void* minimalGetGLProcAddress(const char* name)
{
    void* p = (void*)wglGetProcAddress(name);
    if (p) return p;

    return (void*)GetProcAddress(glModule, name);;
}

void minimalSwapBuffers(MinimalWindow* context)
{
    SwapBuffers(context->deviceContext);
}

void minimalSwapInterval(uint8_t interval)
{
    if (_wglSwapIntervalEXT) _wglSwapIntervalEXT(interval);
}

#endif // !MINIMAL_NO_CONTEXT

#endif // MINIMAL_PLATFORM_WINDOWS