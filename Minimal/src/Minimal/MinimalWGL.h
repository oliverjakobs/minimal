#ifndef MINIMAL_WGL_H
#define MINIMAL_WGL_H

#define MINIMAL_CONTEXT_VERSION_MAJOR
#define MINIMAL_CONTEXT_VERSION_MINOR

#define MINIMAL_OPENGL_PROFILE
#define MINIMAL_OPENGL_CORE_PROFILE

#define MINIMAL_OPENGL_DEBUG_CONTEXT

// See https://www.opengl.org/registry/specs/ARB/wgl_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

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

#include "MinimalWindow.h"

// WGL extension pointer typedefs
typedef BOOL        (WINAPI* wglSwapIntervalEXT_T)(int);
typedef BOOL        (WINAPI* wglChoosePixelFormatARB_T)(HDC, const int*, const float*, UINT, int*, UINT*);
typedef HGLRC       (WINAPI* wglCreateContextAttribsARB_T)(HDC, HGLRC, const int*);

// opengl32.dll function pointer typedefs
typedef HGLRC   (WINAPI* PFN_wglCreateContext)(HDC);
typedef BOOL    (WINAPI* PFN_wglDeleteContext)(HGLRC);
typedef PROC    (WINAPI* PFN_wglGetProcAddress)(LPCSTR);
typedef HDC     (WINAPI* PFN_wglGetCurrentDC)(void);
typedef HGLRC   (WINAPI* PFN_wglGetCurrentContext)(void);
typedef BOOL    (WINAPI* PFN_wglMakeCurrent)(HDC, HGLRC);
typedef BOOL    (WINAPI* PFN_wglShareLists)(HGLRC, HGLRC);

uint8_t MinimalInitWGL();
void MinimalTerminateWGL();

uint8_t MinimalCreateContextWGL(MinimalWindow* window, int major, int minor);

#endif // !MINIMAL_WGL_H
