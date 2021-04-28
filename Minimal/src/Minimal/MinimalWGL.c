#include "MinimalWGL.h"

#include "MinimalWindow.h"

static MinimalBool _min_wgl_initialized = 0;

static wglCreateContextAttribsARB_T _min_wglCreateContextAttrARB = NULL;
static wglSwapIntervalEXT_T         _min_wglSwapIntervalEXT = NULL;
static wglChoosePixelFormatARB_T    _min_wglChoosePixelFormatARB = NULL;

MinimalBool MinimalInitWGL()
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
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to set pixel format for dummy context");
        return MINIMAL_FAIL;
    }

    rc = wglCreateContext(dc);
    if (!rc)
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to create dummy context");
        return MINIMAL_FAIL;
    }

    pdc = wglGetCurrentDC();
    prc = wglGetCurrentContext();

    if (!wglMakeCurrent(dc, rc))
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to make dummy context current");
        wglMakeCurrent(pdc, prc);
        wglDeleteContext(rc);
        return MINIMAL_FAIL;
    }

    _min_wglCreateContextAttrARB = (wglCreateContextAttribsARB_T)wglGetProcAddress("wglCreateContextAttribsARB");
    _min_wglSwapIntervalEXT =      (wglSwapIntervalEXT_T)wglGetProcAddress("wglSwapIntervalEXT");
    _min_wglChoosePixelFormatARB = (wglChoosePixelFormatARB_T)wglGetProcAddress("wglChoosePixelFormatARB");

    _min_wgl_initialized = 1;

    wglMakeCurrent(pdc, prc);
    wglDeleteContext(rc);
    return MINIMAL_OK;
}

void MinimalTerminateWGL()
{
    _min_wgl_initialized = 0;
}

static int MinimalChoosePixelFormat(HDC dc)
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
    _min_wglChoosePixelFormatARB(dc, pf_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats)
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Could not find a suitable pixel format");
        return 0;
    }

    return pixel_format;
}

MinimalBool MinimalCreateContextWGL(MinimalWindow* window, int major, int minor, int flags)
{
    HDC dc = GetDC(MinimalWindowGetHandle(window));
    if (!dc)
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to retrieve device context handle");
        return MINIMAL_FAIL;
    }

    int pixel_format = MinimalChoosePixelFormat(dc);
    if (!pixel_format) return MINIMAL_FAIL;

    PIXELFORMATDESCRIPTOR pfd;
    if (!DescribePixelFormat(dc, pixel_format, sizeof(pfd), &pfd))
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to retrieve PFD");
        return MINIMAL_FAIL;
    }

    if (!SetPixelFormat(dc, pixel_format, &pfd))
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to set pixel format");
        return MINIMAL_FAIL;
    }

    int gl_attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, flags,
        0,
    };

    HGLRC rc = _min_wglCreateContextAttrARB(dc, 0, gl_attribs);
    if (!rc)
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to create render context");
        return MINIMAL_FAIL;
    }

    if (!wglMakeCurrent(dc, rc))
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to make render context current");
        return MINIMAL_FAIL;
    }

    MinimalWindowSetContext(window, dc, rc);

    return MINIMAL_OK;
}

MinimalBool MinimalDestroyContextWGL(MinimalWindow* window)
{
    MinimalBool status = MINIMAL_OK;
    HDC   dc = MinimalWindowGetDeviceContext(window);
    HGLRC rc = MinimalWindowGetRenderContext(window);

    if (rc)
    {
        if (!wglMakeCurrent(NULL, NULL))
        {
            MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to realease render context");
            status = MINIMAL_FAIL;
        }

        if (!wglDeleteContext(rc))
        {
            MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to delete render context");
            status = MINIMAL_FAIL;
        }
    }

    if (dc && !ReleaseDC(MinimalWindowGetHandle(window), dc))
    {
        MinimalErrorCallback(MINIMAL_LOG_ERROR, "WGL", "Failed to release device context");
        status = MINIMAL_FAIL;
    }

    MinimalWindowSetContext(window, NULL, NULL);

    return status;
}

void MinimalSwapIntervalWGL(int interval)
{
    if (_min_wglSwapIntervalEXT) _min_wglSwapIntervalEXT(interval);
}
