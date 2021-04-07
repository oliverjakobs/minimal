

#include "glad/glad.h"

#include <stdio.h>

#include "Minimal/MinimalUtil.h"


typedef struct
{
    HWND handle;
    HDC device_context;
    HGLRC render_context;
    int should_close;
} MinimalWindow;

int MinimalCreateRenderContex(MinimalWindow* window)
{
    PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
            1,                     // version number  
            PFD_DRAW_TO_WINDOW |   // support window  
                PFD_SUPPORT_OPENGL |   // support OpenGL  
                PFD_DOUBLEBUFFER,      // double buffered  
            PFD_TYPE_RGBA,         // RGBA type  
            24,                    // 24-bit color depth  
            0, 0, 0, 0, 0, 0,      // color bits ignored  
            0,                     // no alpha buffer  
            0,                     // shift bit ignored  
            0,                     // no accumulation buffer  
            0, 0, 0, 0,            // accum bits ignored  
            32,                    // 32-bit z-buffer      
            0,                     // no stencil buffer  
            0,                     // no auxiliary buffer  
            PFD_MAIN_PLANE,        // main layer  
            0,                     // reserved  
            0, 0, 0                // layer masks ignored  
    };
    window->device_context = GetDC(window->handle);
    int pf = ChoosePixelFormat(window->device_context, &pfd);
    SetPixelFormat(window->device_context, pf, &pfd);
    window->render_context = wglCreateContext(window->device_context);
    wglMakeCurrent(window->device_context, window->render_context);

    return 1;
}

int MinimalDestroyRenderContext(MinimalWindow* window)
{
    ReleaseDC(window->handle, window->device_context);
    wglDeleteContext(window->render_context);
    return 1;
}

LRESULT MinimalWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MinimalWindow* window = GetPropW(hwnd, L"Minimal");
    switch (msg)
    {
    case WM_DESTROY:
        MinimalDestroyRenderContext(window);
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

int main()
{
    MinimalWindow window;
    MinimalCreateWindow(&window, L"Minimal", 800, 600);

    gladLoadGL();
    glViewport(0, 0, 640, 480);

    while (!window.should_close)
    {
        MinimalPollEvent(&window);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        MinimalSwapBuffer(&window);
    }

    return 0;
}