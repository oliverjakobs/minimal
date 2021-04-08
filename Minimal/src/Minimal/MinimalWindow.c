#include "MinimalWindow.h"

static int MinimalCreateRenderContex(MinimalWindow* window)
{
    window->device_context = GetDC(window->handle);

    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;

    int format = ChoosePixelFormat(window->device_context, &pfd);
    if (!SetPixelFormat(window->device_context, format, &pfd))
    {
        MINIMAL_ERROR("Failed to set pixel format");
        return MINIMAL_FAIL;
    }

    window->render_context = wglCreateContext(window->device_context);
    if (!wglMakeCurrent(window->device_context, window->render_context))
    {
        MINIMAL_ERROR("Failed to create and activate render context");
        return MINIMAL_FAIL;
    }

    return MINIMAL_OK;
}

static int MinimalDestroyRenderContext(MinimalWindow* window)
{
    wglDeleteContext(window->render_context);
    ReleaseDC(window->handle, window->device_context);
    return MINIMAL_OK;
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
    default: return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

int MinimalCreateWindow(MinimalWindow* window, LPCWSTR title, int width, int height)
{
    WCHAR CLASS_NAME[] = L"MinimalWindowClass";
    HINSTANCE instance = GetModuleHandleW(NULL);

    WNDCLASSEXW wnd_class = { 0 };
    wnd_class.cbSize = sizeof(WNDCLASSEXW);
    wnd_class.hInstance = instance;
    wnd_class.lpfnWndProc = MinimalWindowProc;
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

    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    window->handle = CreateWindowExW(0, CLASS_NAME, title, style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, instance, 0);
    window->should_close = 0;

    MINIMAL_INFO("Window created");

    SetPropW(window->handle, L"Minimal", window);

    MinimalCreateRenderContex(window);

    return MINIMAL_OK;
}

int MinimalDestroyWindow(MinimalWindow* window)
{
    return MinimalDestroyRenderContext(window);
}

int MinimalShouldClose(const MinimalWindow* window)
{
    return window->should_close;
}

void MinimalPollEvent(MinimalWindow* window)
{
    MSG msg;
    while (PeekMessageW(&msg, window->handle, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            window->should_close = 0;

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void MinimalSwapBuffer(MinimalWindow* window)
{
    SwapBuffers(window->device_context);
}