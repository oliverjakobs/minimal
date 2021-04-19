#include "MinimalWindow.h"

#include "MinimalInput.h"

#include <windowsx.h>

typedef HGLRC WINAPI wglCreateContextAttribsARB_T(HDC hdc, HGLRC hShareContext, const int* attribList);
wglCreateContextAttribsARB_T* wglCreateContextAttribsARB;

// See https://www.opengl.org/registry/specs/ARB/wgl_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

typedef BOOL WINAPI wglChoosePixelFormatARB_T(HDC hdc, const int* piAttribIList, const float* pfAttribFList, 
                                              UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
wglChoosePixelFormatARB_T* wglChoosePixelFormatARB;

// See https://www.opengl.org/registry/specs/ARB/wgl_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

static uint8_t MinimalLoadGLExtensions()
{
    // Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
    // We use a dummy window because you can only set the pixel format for a window once. For the
    // real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
    // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
    // have a context.

    WCHAR DUMMY_CLASS_NAME[] = L"MinimalDummyWindowClass";
    HINSTANCE instance = GetModuleHandleW(NULL);

    WNDCLASSW dummy_class = { 0 };
    dummy_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    dummy_class.lpfnWndProc = DefWindowProcA;
    dummy_class.hInstance = instance;
    dummy_class.lpszClassName = DUMMY_CLASS_NAME;

    if (!RegisterClassW(&dummy_class))
    {
        MINIMAL_ERROR("Failed to register dummy window class.");
        return MINIMAL_FAIL;
    }

    HWND dummy_window = CreateWindowExW(0, DUMMY_CLASS_NAME, NULL, 0, 0, 0, 0, 0, 0, 0, instance, 0);
    if (!dummy_window)
    {
        MINIMAL_ERROR("Failed to create dummy OpenGL window.");
        return MINIMAL_FAIL;
    }

    HDC dummy_dc = GetDC(dummy_window);

    PIXELFORMATDESCRIPTOR pfd = {
        .nSize = sizeof(pfd),
        .nVersion = 1,
        .iPixelType = PFD_TYPE_RGBA,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .cColorBits = 32,
        .cAlphaBits = 8,
        .iLayerType = PFD_MAIN_PLANE,
        .cDepthBits = 24,
        .cStencilBits = 8,
    };

    int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
    if (!pixel_format)
    {
        MINIMAL_ERROR("Failed to find a suitable pixel format.");
        return MINIMAL_FAIL;
    }
    if (!SetPixelFormat(dummy_dc, pixel_format, &pfd))
    {
        MINIMAL_ERROR("Failed to set the pixel format.");
        return MINIMAL_FAIL;
    }

    HGLRC dummy_context = wglCreateContext(dummy_dc);
    if (!dummy_context)
    {
        MINIMAL_ERROR("Failed to create a dummy rendering context.");
        return MINIMAL_FAIL;
    }

    if (!wglMakeCurrent(dummy_dc, dummy_context))
    {
        MINIMAL_ERROR("Failed to activate dummy rendering context.");
        return MINIMAL_FAIL;
    }

    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_T*)wglGetProcAddress("wglCreateContextAttribsARB");
    wglChoosePixelFormatARB =    (wglChoosePixelFormatARB_T*)wglGetProcAddress("wglChoosePixelFormatARB");

    wglMakeCurrent(dummy_dc, 0);
    wglDeleteContext(dummy_context);
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);

    return MINIMAL_OK;
}

static uint8_t MinimalCreateRenderContex(MinimalWindow* wnd, int gl_major, int gl_minor)
{
    if (!MinimalLoadGLExtensions()) return MINIMAL_FAIL;

    wnd->device_context = GetDC(wnd->handle);
    if (!wnd->device_context)
    {
        MINIMAL_ERROR("Failed to retrieve device context handle");
        return MINIMAL_FAIL;
    }

    // Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
    int pixel_format_attribs[] = {
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
    wglChoosePixelFormatARB(wnd->device_context, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats)
    {
        MINIMAL_ERROR("Could not find a suitable pixel format");
        return MINIMAL_FAIL;
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(wnd->device_context, pixel_format, sizeof(pfd), &pfd);
    if (!SetPixelFormat(wnd->device_context, pixel_format, &pfd))
    {
        MINIMAL_ERROR("Failed to set pixel format");
        return MINIMAL_FAIL;
    }

    // Specify that we want to create an OpenGL 3.3 core profile context
    int gl_attribs[] = 
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, gl_major,
        WGL_CONTEXT_MINOR_VERSION_ARB, gl_minor,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    wnd->render_context = wglCreateContextAttribsARB(wnd->device_context, 0, gl_attribs);
    if (!wnd->render_context)
    {
        MINIMAL_ERROR("Failed to create render context");
        return MINIMAL_FAIL;
    }

    if (!wglMakeCurrent(wnd->device_context, wnd->render_context))
    {
        MINIMAL_ERROR("Failed to make render context current");
        return MINIMAL_FAIL;
    }

    return MINIMAL_OK;
}

static uint8_t MinimalDestroyRenderContext(MinimalWindow* wnd)
{
    uint8_t status = MINIMAL_OK;
    if (wnd->render_context)
    {
        if (!wglMakeCurrent(NULL, NULL))
        {
            MINIMAL_ERROR("Failed to realease render context");
            status = MINIMAL_FAIL;
        }

        if (!wglDeleteContext(wnd->render_context))
        {
            MINIMAL_ERROR("Failed to delete render context");
            status = MINIMAL_FAIL;
        }
    }

    if (wnd->device_context && !ReleaseDC(wnd->handle, wnd->device_context))
    {
        MINIMAL_ERROR("Failed to release device context");
        status = MINIMAL_FAIL;
    }

    wnd->render_context = NULL;
    wnd->device_context = NULL;

    return status;
}

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
    int i;
    for (i = 0; i <= MINIMAL_MOUSE_BUTTON_LAST; i++)
    {
        if (window->mouse_buttons[i] == MINIMAL_PRESS)
            break;
    }
    return i;
}

static UINT MinimalGetMouseButton(UINT msg, WPARAM wParam)
{
    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)   return MINIMAL_MOUSE_BUTTON_LEFT;
    if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)   return MINIMAL_MOUSE_BUTTON_RIGHT;
    if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)   return MINIMAL_MOUSE_BUTTON_MIDDLE;
    if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)         return MINIMAL_MOUSE_BUTTON_4;
    return MINIMAL_MOUSE_BUTTON_5;
}

static void MinimalUpdateKey(MinimalWindow* window, UINT key, UINT scancode, UINT action, UINT mods)
{
    if (MinimalKeycodeValid(key))
    {
        uint8_t prev = window->key_state[key];
        if (action == MINIMAL_PRESS && prev == MINIMAL_RELEASE)     window->key_state[key] = MINIMAL_PRESS;
        else if (action == MINIMAL_PRESS && prev == MINIMAL_PRESS)  window->key_state[key] = MINIMAL_REPEAT;
        else if (action == MINIMAL_RELEASE)                         window->key_state[key] = MINIMAL_RELEASE;
    }

    if (window->callbacks.key) window->callbacks.key(window, key, scancode, action, mods);
}

static void MinimalUpdateMouseButton(MinimalWindow* window, UINT button, UINT action, UINT mods)
{
    if (MinimalMouseButtonValid(button)) window->mouse_buttons[button] = (uint8_t)action;
    if (window->callbacks.m_button) window->callbacks.m_button(window, button, action, mods);
}

static void MinimalUpdateCursorPos(MinimalWindow* window, float x, float y)
{
    if (window->callbacks.cursor_pos) window->callbacks.cursor_pos(window, x, y);
}

static LRESULT MinimalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MinimalWindow* window = GetPropW(hwnd, L"Minimal");
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
        const uint8_t plain = (msg != WM_SYSCHAR);

        // _glfwInputChar(window, (unsigned int)wParam, getKeyMods(), plain);
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

        MinimalUpdateKey(window, keycode, scancode, action, mods);
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
        const UINT action = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || 
                             msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN) ? MINIMAL_PRESS : MINIMAL_RELEASE;
        const UINT mods = MinimalGetKeyMods();

        if (MinimalCheckMouseButtons(window) > MINIMAL_MOUSE_BUTTON_LAST) SetCapture(hwnd);

        MinimalUpdateMouseButton(window, button, action, mods);

        if (MinimalCheckMouseButtons(window) > MINIMAL_MOUSE_BUTTON_LAST) ReleaseCapture();

        return msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP;
    }
    case WM_MOUSEMOVE:
    {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);

        MinimalUpdateCursorPos(window, (float)x, (float)y);
        return 0;
    }
    default: return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

uint8_t MinimalCreateWindow(MinimalWindow* wnd, const char* title, uint32_t width, uint32_t height)
{
    WCHAR CLASS_NAME[] = L"MinimalWindowClass";
    wnd->instance = GetModuleHandleW(NULL);

    WNDCLASSEXW wnd_class = { 0 };
    wnd_class.cbSize = sizeof(WNDCLASSEXW);
    wnd_class.hInstance = wnd->instance;
    wnd_class.lpfnWndProc = MinimalWindowProc;
    wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wnd_class.lpszClassName = CLASS_NAME;
    wnd_class.lpszMenuName = NULL;
    wnd_class.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wnd_class.hIconSm = LoadIconW(NULL, IDI_APPLICATION);
    wnd_class.hCursor = LoadCursorW(NULL, IDC_ARROW);

    if (!RegisterClassExW(&wnd_class))
    {
        MINIMAL_ERROR("Failed to register WindowClass");
        return MINIMAL_FAIL;
    }

    RECT rect = { .right = width, .bottom = height };
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    AdjustWindowRect(&rect, style, 0);

    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    int w = rect.right - rect.left, h = rect.bottom - rect.top;

    wnd->handle = CreateWindowExW(0, CLASS_NAME, NULL, style, x, y, w, h, 0, 0, wnd->instance, 0);
    wnd->should_close = 0;

    MinimalSetWindowTitle(wnd, title);
    SetPropW(wnd->handle, L"Minimal", wnd);

    memset(wnd->key_state, MINIMAL_RELEASE, sizeof(wnd->key_state));
    memset(wnd->mouse_buttons, MINIMAL_RELEASE, sizeof(wnd->mouse_buttons));

    wnd->callbacks.size = NULL;
    wnd->callbacks.close = NULL;
    wnd->callbacks.key = NULL;
    wnd->callbacks.character = NULL;
    wnd->callbacks.m_button = NULL;
    wnd->callbacks.scroll = NULL;
    wnd->callbacks.cursor_pos = NULL;

    return MinimalCreateRenderContex(wnd, 4, 3);
}

uint8_t MinimalDestroyWindow(MinimalWindow* wnd)
{
    int status = MinimalDestroyRenderContext(wnd);

    if (wnd->handle && !DestroyWindow(wnd->handle))
    {
        MINIMAL_ERROR("Failed to destroy window");
        status = MINIMAL_FAIL;
    }

    if (!UnregisterClassW(L"MinimalWindowClass", wnd->instance))
    {
        MINIMAL_ERROR("Failed to unregister WindowClass");
        status = MINIMAL_FAIL;
    }

    wnd->handle = NULL;
    wnd->instance = NULL;

    return status;
}

void MinimalPollEvent(MinimalWindow* wnd)
{
    MSG msg;
    while (PeekMessageW(&msg, wnd->handle, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            MinimalCloseWindow(wnd);

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void MinimalSwapBuffer(MinimalWindow* wnd)
{
    SwapBuffers(wnd->device_context);
}

void MinimalSetWindowTitle(MinimalWindow* wnd, const char* str)
{
    SetWindowTextA(wnd->handle, str);
}

uint32_t MinimalGetWindowWidth(const MinimalWindow* wnd)
{
    RECT rect;
    return GetWindowRect(wnd->handle, &rect) ? rect.right - rect.left : 0;
}

uint32_t MinimalGetWindowHeigth(const MinimalWindow* wnd)
{
    RECT rect;
    return GetWindowRect(wnd->handle, &rect) ? rect.bottom - rect.top : 0;
}

uint8_t MinimalShouldClose(const MinimalWindow* wnd)    { return wnd->should_close; }
void MinimalCloseWindow(MinimalWindow* wnd)             { wnd->should_close = 1; }

void MinimalSetSizeCallback(MinimalWindow* wnd, MinimalSizeCB cb)           { wnd->callbacks.size = cb; }
void MinimalSetCloseCallback(MinimalWindow* wnd, MinimalCloseCB cb)         { wnd->callbacks.close = cb; }
void MinimalSetKeyCallback(MinimalWindow* wnd, MinimalKeyCB cb)             { wnd->callbacks.key = cb; }
void MinimalSetCharCallback(MinimalWindow* wnd, MinimalCharCB cb)           { wnd->callbacks.character = cb; }
void MinimalSetMButtonCallback(MinimalWindow* wnd, MinimalMButtonCB cb)     { wnd->callbacks.m_button = cb; }
void MinimalSetScrollCallback(MinimalWindow* wnd, MinimalScrollCB cb)       { wnd->callbacks.scroll = cb; }
void MinimalSetCursorPosCallback(MinimalWindow* wnd, MinimalCursorPosCB cb) { wnd->callbacks.cursor_pos = cb; }
