#include "MinimalWGL.h"

#include "MinimalWindow.h"

static uint8_t _min_wgl_initialized = 0;

static wglCreateContextAttribsARB_T    _min_wglCreateContextAttribsARB = NULL;
static wglSwapIntervalEXT_T            _min_wglSwapIntervalEXT = NULL;
static wglChoosePixelFormatARB_T       _min_wglChoosePixelFormatARB = NULL;

uint8_t MinimalInitWGL()
{
    if (_min_wgl_initialized) return MINIMAL_OK;

    HGLRC prc, rc;
    HDC pdc, dc;

    dc = GetDC(MinimalGetHelperWindow());

    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;

    if (!SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd))
    {
        MINIMAL_ERROR("WGL: Failed to set pixel format for dummy context");
        return MINIMAL_FAIL;
    }

    rc = wglCreateContext(dc);
    if (!rc)
    {
        MINIMAL_ERROR("WGL: Failed to create dummy context");
        return MINIMAL_FAIL;
    }

    pdc = wglGetCurrentDC();
    prc = wglGetCurrentContext();

    if (!wglMakeCurrent(dc, rc))
    {
        MINIMAL_ERROR("WGL: Failed to make dummy context current");
        wglMakeCurrent(pdc, prc);
        wglDeleteContext(rc);
        return MINIMAL_FAIL;
    }

    _min_wglCreateContextAttribsARB = (wglCreateContextAttribsARB_T)wglGetProcAddress("wglCreateContextAttribsARB");
    _min_wglSwapIntervalEXT =         (wglSwapIntervalEXT_T)wglGetProcAddress("wglSwapIntervalEXT");
    _min_wglChoosePixelFormatARB =    (wglChoosePixelFormatARB_T)wglGetProcAddress("wglChoosePixelFormatARB");

    _min_wgl_initialized = 1;

    wglMakeCurrent(pdc, prc);
    wglDeleteContext(rc);
    return MINIMAL_OK;
}

void MinimalTerminateWGL()
{
    _min_wgl_initialized = 0;
}

static int MinimalChoosePixelFormat(MinimalWindow* window)
{
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
    _min_wglChoosePixelFormatARB(window->device_context, pf_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats)
    {
        MINIMAL_ERROR("Could not find a suitable pixel format");
        return 0;
    }

    return pixel_format;
}

uint8_t MinimalCreateContextWGL(MinimalWindow* window, int major, int minor)
{
    window->device_context = GetDC(window->handle);
    if (!window->device_context)
    {
        MINIMAL_ERROR("Failed to retrieve device context handle");
        return MINIMAL_FAIL;
    }

    int pixel_format = MinimalChoosePixelFormat(window);
    if (!pixel_format) return MINIMAL_FAIL;

    PIXELFORMATDESCRIPTOR pfd;
    if (!DescribePixelFormat(window->device_context, pixel_format, sizeof(pfd), &pfd))
    {
        MINIMAL_ERROR("Failed to retrieve PFD");
        return MINIMAL_FAIL;
    }

    if (!SetPixelFormat(window->device_context, pixel_format, &pfd))
    {
        MINIMAL_ERROR("Failed to set pixel format");
        return MINIMAL_FAIL;
    }
    // Specify that we want to create an OpenGL 3.3 core profile context
    int gl_attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    window->render_context = _min_wglCreateContextAttribsARB(window->device_context, 0, gl_attribs);
    if (!window->render_context)
    {
        MINIMAL_ERROR("Failed to create render context");
        return MINIMAL_FAIL;
    }

    if (!wglMakeCurrent(window->device_context, window->render_context))
    {
        MINIMAL_ERROR("Failed to make render context current");
        return MINIMAL_FAIL;
    }

    return MINIMAL_OK;
}

uint8_t MinimalDestroyContextWGL(MinimalWindow* window)
{
    uint8_t status = MINIMAL_OK;
    if (window->render_context)
    {
        if (!wglMakeCurrent(NULL, NULL))
        {
            MINIMAL_ERROR("Failed to realease render context");
            status = MINIMAL_FAIL;
        }

        if (!wglDeleteContext(window->render_context))
        {
            MINIMAL_ERROR("Failed to delete render context");
            status = MINIMAL_FAIL;
        }
    }

    if (window->device_context && !ReleaseDC(window->handle, window->device_context))
    {
        MINIMAL_ERROR("Failed to release device context");
        status = MINIMAL_FAIL;
    }

    window->render_context = NULL;
    window->device_context = NULL;

    return status;
}
