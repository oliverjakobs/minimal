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
        const UINT key = MinimalTranslateKey((UINT)wParam);
        const UINT scancode = MapVirtualKeyA((UINT)wParam, MAPVK_VK_TO_VSC);
        const UINT action = ((lParam >> 31) & 1) ? 0 : 1;
        const UINT mods = MinimalGetKeyMods();

        printf("key: %X\n", key);
        printf("scancode: %X\n", scancode);
        printf("action: %d\n", action);
        printf("mods : %d\n", mods);
        // _glfwInputKey(window, scancode, action, mods);
        return 0;
    }
    default: return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

int MinimalCreateWindow(MinimalWindow* wnd, LPCWSTR title, int width, int height)
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
    wnd->handle = CreateWindowExW(0, CLASS_NAME, title, style, x, y, width, height, 0, 0, wnd->instance, 0);
    wnd->should_close = 0;

    SetPropW(wnd->handle, L"Minimal", wnd);

    MinimalCreateRenderContex(wnd);

    return MINIMAL_OK;
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
            wnd->should_close = 0;

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void MinimalSwapBuffer(MinimalWindow* wnd)
{
    SwapBuffers(wnd->device_context);
}

int MinimalShouldClose(const MinimalWindow* wnd)    { return wnd->should_close; }
void MinimalCloseWindow(MinimalWindow* wnd)         { wnd->should_close = 1; }