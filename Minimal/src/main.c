#include <Windows.h>

#include "glad/glad.h"

#include <stdio.h>

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define MINIMAL_TRUE    1
#define MINIMAL_FALSE   0

HWND window_handle;
HDC dc;
HGLRC rc;

typedef struct
{
    HWND handle;
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
    dc = GetDC(window->handle);
    int pf = ChoosePixelFormat(dc, &pfd);
    SetPixelFormat(dc, pf, &pfd);
    rc = wglCreateContext(dc);
    wglMakeCurrent(dc, rc);

    return 1;
}

int MinimalDestroyRenderContext(MinimalWindow* window)
{
    ReleaseDC(window->handle, dc);
    wglDeleteContext(rc);
    return 1;
}

void MinimalCreateWindow(MinimalWindow* window, LPCWSTR title, int width, int height)
{
    wchar_t CLASS_NAME[] = L"MinimalWindowClass";
    HINSTANCE instance = GetModuleHandleW(NULL);

    WNDCLASS wnd_class = { 0 };
    wnd_class.hInstance = instance;
    wnd_class.lpfnWndProc = WindowProc;
    wnd_class.lpszClassName = CLASS_NAME;

    RegisterClassW(&wnd_class);

    window->handle = CreateWindowExW(0, CLASS_NAME, title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, instance, 0);
    window->should_close = 0;

    SetPropW(window->handle, L"Minimal", window);

    MinimalCreateRenderContex(window);
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
        SwapBuffers(dc);
    }

    return 0;
}

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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