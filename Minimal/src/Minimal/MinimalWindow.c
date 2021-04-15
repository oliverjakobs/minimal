#include "MinimalWindow.h"

#include "MinimalInput.h"

static int MinimalCreateRenderContex(MinimalWindow* wnd)
{
    wnd->device_context = GetDC(wnd->handle);
    if (!wnd->device_context)
    {
        MINIMAL_ERROR("Failed to retrieve device context handle");
        return MINIMAL_FAIL;
    }

    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int format = ChoosePixelFormat(wnd->device_context, &pfd);
    if (!format)
    {
        MINIMAL_ERROR("Could not find a suitable pixel format");
        return MINIMAL_FAIL;
    }

    if (!SetPixelFormat(wnd->device_context, format, &pfd))
    {
        MINIMAL_ERROR("Failed to set pixel format");
        return MINIMAL_FAIL;
    }

    wnd->render_context = wglCreateContext(wnd->device_context);
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

static int MinimalDestroyRenderContext(MinimalWindow* wnd)
{
    int status = MINIMAL_OK;
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

static UINT MinimalGetKeyMods()
{
    UINT mods = 0;
    if (GetKeyState(VK_SHIFT) & 0x8000)                         mods |= MINIMAL_KEY_MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) & 0x8000)                       mods |= MINIMAL_KEY_MOD_CONTROL;
    if (GetKeyState(VK_MENU) & 0x8000)                          mods |= MINIMAL_KEY_MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) mods |= MINIMAL_KEY_MOD_SUPER;
    if (GetKeyState(VK_CAPITAL) & 1)                            mods |= MINIMAL_KEY_MOD_CAPS_LOCK;
    if (GetKeyState(VK_NUMLOCK) & 1)                            mods |= MINIMAL_KEY_MOD_NUM_LOCK;
    return mods;
}

void MinimalUpdateKey(MinimalWindow* window, UINT keycode, UINT scancode, MinimalInputAction action, UINT mods)
{
    if (MinimalKeycodeValid(keycode))
    {
        MinimalInputAction prev = window->key_state[keycode];
        if (action == MINIMAL_PRESS && prev == MINIMAL_RELEASE)     window->key_state[keycode] = MINIMAL_PRESS;
        else if (action == MINIMAL_PRESS && prev == MINIMAL_PRESS)  window->key_state[keycode] = MINIMAL_REPEAT;
        else if (action == MINIMAL_RELEASE)                         window->key_state[keycode] = MINIMAL_RELEASE;
    }

    if (window->callbacks.key) window->callbacks.key(window, keycode, scancode, action, mods);
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
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        const UINT scancode = (lParam >> 16) & 0x1ff;
        const UINT keycode = MinimalTranslateKey(scancode);
        const MinimalInputAction action = ((lParam >> 31) & 1) ? MINIMAL_RELEASE : MINIMAL_PRESS;
        const UINT mods = MinimalGetKeyMods();

        MinimalUpdateKey(window, keycode, scancode, action, mods);
        return 0;
    }
    default: return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

int MinimalCreateWindow(MinimalWindow* wnd, const char* title, int width, int height)
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

    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    wnd->handle = CreateWindowExW(0, CLASS_NAME, NULL, style, x, y, width, height, 0, 0, wnd->instance, 0);
    wnd->should_close = 0;

    MinimalSetWindowTitle(wnd, title);
    SetPropW(wnd->handle, L"Minimal", wnd);

    memset(wnd->key_state, MINIMAL_RELEASE, sizeof(wnd->key_state));

    wnd->callbacks.size = NULL;
    wnd->callbacks.close = NULL;
    wnd->callbacks.key = NULL;
    wnd->callbacks.character = NULL;
    wnd->callbacks.mouse_button = NULL;
    wnd->callbacks.scroll = NULL;
    wnd->callbacks.cursor_pos = NULL;

    return MinimalCreateRenderContex(wnd);
}

int MinimalDestroyWindow(MinimalWindow* wnd)
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

long MinimalGetWindowWidth(const MinimalWindow* wnd)
{
    RECT rect;
    return GetWindowRect(wnd->handle, &rect) ? rect.right - rect.left : 0;
}

long MinimalGetWindowHeigth(const MinimalWindow* wnd)
{
    RECT rect;
    return GetWindowRect(wnd->handle, &rect) ? rect.bottom - rect.top : 0;
}

int MinimalShouldClose(const MinimalWindow* wnd)    { return wnd->should_close; }
void MinimalCloseWindow(MinimalWindow* wnd)         { wnd->should_close = 1; }