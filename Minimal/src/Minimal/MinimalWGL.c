#include "MinimalWGL.h"

#include "MinimalWindow.h"

HINSTANCE _min_wgl_instance = NULL;
PFN_wglCreateContext        _min_wgl_create_context = NULL;
PFN_wglDeleteContext        _min_wgl_delete_context = NULL;
PFN_wglGetProcAddress       _min_wgl_get_proc_address = NULL;
PFN_wglGetCurrentDC         _min_wgl_get_current_dc = NULL;
PFN_wglGetCurrentContext    _min_wgl_get_current = NULL;
PFN_wglMakeCurrent          _min_wgl_make_current = NULL;
PFN_wglShareLists           _min_wgl_share_lists = NULL;

wglCreateContextAttribsARB_T    _min_wgl_create_context_attribs_ARB = NULL;
wglSwapIntervalEXT_T            _min_wgl_swap_interval_EXT = NULL;
wglChoosePixelFormatARB_T       _min_wgl_choose_pixel_format_ARB = NULL;

uint8_t MinimalInitWGL()
{
    HGLRC prc, rc;
    HDC pdc, dc;

    if (_min_wgl_instance) return MINIMAL_OK;

    _min_wgl_instance = LoadLibraryA("opengl32.dll");
    if (!_min_wgl_instance)
    {
        MINIMAL_ERROR("WGL: Failed to load opengl32.dll");
        return MINIMAL_FAIL;
    }

    _min_wgl_create_context = (PFN_wglCreateContext)GetProcAddress(_min_wgl_instance, "wglCreateContext");
    _min_wgl_delete_context = (PFN_wglDeleteContext)GetProcAddress(_min_wgl_instance, "wglDeleteContext");
    _min_wgl_get_proc_address = (PFN_wglGetProcAddress)GetProcAddress(_min_wgl_instance, "wglGetProcAddress");
    _min_wgl_get_current_dc = (PFN_wglGetCurrentDC)GetProcAddress(_min_wgl_instance, "wglGetCurrentDC");
    _min_wgl_get_current = (PFN_wglGetCurrentContext)GetProcAddress(_min_wgl_instance, "wglGetCurrentContext");
    _min_wgl_make_current = (PFN_wglMakeCurrent)GetProcAddress(_min_wgl_instance, "wglMakeCurrent");
    _min_wgl_share_lists = (PFN_wglShareLists)GetProcAddress(_min_wgl_instance, "wglShareLists");

    // NOTE: A dummy context has to be created for opengl32.dll to load the
    //       OpenGL ICD, from which we can then query WGL extensions
    // NOTE: This code will accept the Microsoft GDI ICD; accelerated context
    //       creation failure occurs during manual pixel format enumeration

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

    _min_wgl_create_context_attribs_ARB = (wglCreateContextAttribsARB_T)wglGetProcAddress("wglCreateContextAttribsARB");
    _min_wgl_swap_interval_EXT = (wglSwapIntervalEXT_T)wglGetProcAddress("wglSwapIntervalEXT");
    _min_wgl_choose_pixel_format_ARB = (wglChoosePixelFormatARB_T)wglGetProcAddress("wglChoosePixelFormatARB_T");

    wglMakeCurrent(pdc, prc);
    wglDeleteContext(rc);
    return MINIMAL_OK;
}

void MinimalTerminateWGL()
{
    if (_min_wgl_instance) FreeLibrary(_min_wgl_instance);
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
    _min_wgl_choose_pixel_format_ARB(window->device_context, pf_attribs, 0, 1, &pixel_format, &num_formats);
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

    window->render_context = _min_wgl_create_context_attribs_ARB(window->device_context, 0, gl_attribs);
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
