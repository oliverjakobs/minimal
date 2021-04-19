#include "MinimalWindow.h"

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

uint8_t MinimalRegisterWindowClass()
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
        MINIMAL_ERROR("Failed to register WindowClass");
        return MINIMAL_FAIL;
    }
    return MINIMAL_OK;
}

uint8_t MinimalUnregisterWindowClass()
{
    if (!UnregisterClassW(L"MinimalWindowClass", GetModuleHandleW(NULL)))
    {
        MINIMAL_ERROR("Failed to unregister WindowClass");
        return MINIMAL_FAIL;
    }
    return MINIMAL_OK;
}

static HWND _minimal_helper_hwnd;

uint8_t MinimalCreateHelperWindow()
{
    DWORD ex_style = WS_EX_OVERLAPPEDWINDOW;
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    _minimal_helper_hwnd = CreateWindowExW(ex_style, MINIMAL_WNDCLASSNAME, NULL, style,
            0, 0, 1, 1, 0, 0, GetModuleHandleW(NULL), 0);

    if (!_minimal_helper_hwnd)
    {
        MINIMAL_ERROR("Failed to create helper window");
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

HWND MinimalGetHelperWindow()
{
    return _minimal_helper_hwnd;
}

uint8_t MinimalCreateWindow(MinimalWindow* wnd, const char* title, uint32_t width, uint32_t height)
{
    wnd->instance = GetModuleHandleW(NULL);

    RECT rect = { .right = width, .bottom = height };
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    AdjustWindowRect(&rect, style, 0);

    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    int w = rect.right - rect.left, h = rect.bottom - rect.top;

    wnd->handle = CreateWindowExW(0, MINIMAL_WNDCLASSNAME, NULL, style, x, y, w, h, 0, 0, wnd->instance, 0);
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

    wnd->handle = NULL;
    wnd->instance = NULL;

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

uint8_t MinimalShouldCloseWindow(const MinimalWindow* wnd)  { return wnd->should_close; }
void MinimalCloseWindow(MinimalWindow* wnd)                 { wnd->should_close = 1; }

void MinimalSetSizeCallback(MinimalWindow* wnd, MinimalSizeCB cb)           { wnd->callbacks.size = cb; }
void MinimalSetCloseCallback(MinimalWindow* wnd, MinimalCloseCB cb)         { wnd->callbacks.close = cb; }
void MinimalSetKeyCallback(MinimalWindow* wnd, MinimalKeyCB cb)             { wnd->callbacks.key = cb; }
void MinimalSetCharCallback(MinimalWindow* wnd, MinimalCharCB cb)           { wnd->callbacks.character = cb; }
void MinimalSetMButtonCallback(MinimalWindow* wnd, MinimalMButtonCB cb)     { wnd->callbacks.m_button = cb; }
void MinimalSetScrollCallback(MinimalWindow* wnd, MinimalScrollCB cb)       { wnd->callbacks.scroll = cb; }
void MinimalSetCursorPosCallback(MinimalWindow* wnd, MinimalCursorPosCB cb) { wnd->callbacks.cursor_pos = cb; }


/* --------------------------| Input |----------------------------------- */
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

int8_t MinimalGetKeyState(const MinimalWindow* window, uint32_t keycode)
{
    return (window && MinimalKeycodeValid(keycode)) ? window->key_state[keycode] : -1;
}

int8_t MinimalGetMouseButtonState(const MinimalWindow* window, uint32_t button)
{
    return (window && MinimalMouseButtonValid(button)) ? window->mouse_buttons[button] : -1;
}
