#include "platform.h"

#ifdef MINIMAL_PLATFORM_GLFW

#include <GLFW/glfw3.h>
#include "event.h"

uint8_t minimalPlatformInit()       { return glfwInit(); }
uint8_t minimalPlatformTerminate()  { glfwTerminate(); return MINIMAL_OK; }

static void minimalGLFWErrorCallback(int error, const char* desc)
{
    MINIMAL_ERROR("[GLFW] (%d) %s", error, desc);
}

static void minimalGLFWWindowSizeCallback(GLFWwindow* window, int width, int height);
static void minimalGLFWWindowIconifyCallback(GLFWwindow* window, int iconified);
static void minimalGLFWWindowMaximizeCallback(GLFWwindow* window, int maximized);
static void minimalGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void minimalGLFWCharCallback(GLFWwindow* window, unsigned int keycode);
static void minimalGLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void minimalGLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
static void minimalGLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

MinimalWindow* minimalCreateWindow(const char* title, uint32_t w, uint32_t h, const MinimalWndConfig* config)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config->gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config->gl_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    glfwSetErrorCallback(minimalGLFWErrorCallback);

    GLFWwindow* window = glfwCreateWindow(w, h, title, NULL, NULL);
    if (!window) return NULL;

    /* set GLFW callbacks */
    glfwSetWindowSizeCallback(window,     minimalGLFWWindowSizeCallback);
    glfwSetWindowIconifyCallback(window,  minimalGLFWWindowIconifyCallback);
    glfwSetWindowMaximizeCallback(window, minimalGLFWWindowMaximizeCallback);
    glfwSetKeyCallback(window,            minimalGLFWKeyCallback);
    glfwSetCharCallback(window,           minimalGLFWCharCallback);
    glfwSetMouseButtonCallback(window,    minimalGLFWMouseButtonCallback);
    glfwSetCursorPosCallback(window,      minimalGLFWCursorPosCallback);
    glfwSetScrollCallback(window,         minimalGLFWScrollCallback);

    return (MinimalWindow*)window;
}

void minimalDestroyWindow(MinimalWindow* window) { glfwDestroyWindow((GLFWwindow*)window); }

void  minimalSetWindowEventHandler(MinimalWindow* window, void* handler)
{
    glfwSetWindowUserPointer((GLFWwindow*)window, handler);
}

void* minimalGetWindowEventHandler(MinimalWindow* window)
{
    return glfwGetWindowUserPointer((GLFWwindow*)window);
}

void minimalMakeContextCurrent(MinimalWindow* context) { glfwMakeContextCurrent((GLFWwindow*)context); }
MinimalWindow* minimalGetCurrentContext()              { return (MinimalWindow*)glfwGetCurrentContext(); }

void minimalPollEvent(MinimalWindow* context)   { glfwPollEvents(); }
void minimalSwapBuffers(MinimalWindow* context) { glfwSwapBuffers((GLFWwindow*)context); }

void minimalSetWindowTitle(MinimalWindow* context, const char* title)
{
    glfwSetWindowTitle((GLFWwindow*)context, title);
}

void minimalSwapInterval(uint8_t interval) { glfwSwapInterval(interval); }

uint8_t minimalShouldClose(const MinimalWindow* context) { return glfwWindowShouldClose((GLFWwindow*)context); };
void    minimalCloseWindow(MinimalWindow* context) { glfwSetWindowShouldClose((GLFWwindow*)context, GLFW_TRUE); }

void minimalGetContentSize(const MinimalWindow* context, int32_t* w, int32_t* h)
{
    glfwGetWindowSize((GLFWwindow*)context, w, w);
}

void minimalGetWindowSize(const MinimalWindow* context, int32_t* w, int32_t* h)
{
    int left, top, right, bottom;
    glfwGetWindowFrameSize((GLFWwindow*)context, &left, &top, &right, &bottom);

    if (w) *w = right - left;
    if (h) *h = bottom - top;
}

int8_t minimalGetKeyState(const MinimalWindow* context, uint32_t keycode)
{
    return glfwGetKey((GLFWwindow*)context, keycode);
}

int8_t minimalGetMouseButtonState(const MinimalWindow* context, uint32_t button)
{
    return glfwGetMouseButton((GLFWwindow*)context, button);
}

void minimalGetCursorPos(const MinimalWindow* context, float* x, float* y)
{
    double xpos, ypos;
    glfwGetCursorPos((GLFWwindow*)context, &xpos, &ypos);

    if (x) *x = (float)xpos;
    if (y) *y = (float)ypos;
}

double minimalGetTime() { return glfwGetTime(); }

/* --------------------------| glfw events |----------------------------- */
void minimalGLFWWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    MinimalApp* app = (MinimalApp*)glfwGetWindowUserPointer(window);
    if (app) minimalDispatchEvent(app, MINIMAL_EVENT_WINDOW_SIZE, 0, width, height);
}

void minimalGLFWWindowIconifyCallback(GLFWwindow* window, int iconified)
{
    MinimalApp* app = (MinimalApp*)glfwGetWindowUserPointer(window);
    if (app) minimalDispatchEvent(app, MINIMAL_EVENT_WINDOW_MINIMIZE, iconified, 0, 0);
}

void minimalGLFWWindowMaximizeCallback(GLFWwindow* window, int maximized)
{
    MinimalApp* app = (MinimalApp*)glfwGetWindowUserPointer(window);
    if (app) minimalDispatchEvent(app, MINIMAL_EVENT_WINDOW_MAXIMIZE, maximized, 0, 0);
}

void minimalGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    MinimalApp* app = (MinimalApp*)glfwGetWindowUserPointer(window);
    if (app) minimalDispatchEvent(app, MINIMAL_EVENT_KEY, key, action, mods);
}

void minimalGLFWCharCallback(GLFWwindow* window, unsigned int keycode)
{
    MinimalApp* app = (MinimalApp*)glfwGetWindowUserPointer(window);
    if (app) minimalDispatchEvent(app, MINIMAL_EVENT_CHAR, keycode, 0, 0);
}

void minimalGLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    MinimalApp* app = (MinimalApp*)glfwGetWindowUserPointer(window);
    if (!app) return;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    minimalDispatchEvent(app, MINIMAL_EVENT_MOUSE_BUTTON, (button << 16) + action, (int32_t)xpos, (int32_t)ypos);
}

void minimalGLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    MinimalApp* app = (MinimalApp*)glfwGetWindowUserPointer(window);
    if (app) minimalDispatchEvent(app, MINIMAL_EVENT_MOUSE_MOVED, 0, (int32_t)xpos, (int32_t)ypos);
}

void minimalGLFWScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    MinimalApp* app = (MinimalApp*)glfwGetWindowUserPointer(window);
    if (app) minimalDispatchEvent(app, MINIMAL_EVENT_MOUSE_SCROLLED, 0, (int32_t)xOffset, (int32_t)yOffset);
}

#endif // MINIMAL_PLATFORM_GLFW