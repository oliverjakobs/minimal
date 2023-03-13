#include "application.h"

#include <string.h>

/* --------------------------| minimal app |----------------------------- */
static void minimalGLFWErrorCallback(int error, const char* desc)
{
    MINIMAL_ERROR("[GLFW] (%d) %s", error, desc);
}

void minimalGLFWWindowSizeCallback(GLFWwindow* window, int width, int height);

void minimalGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void minimalGLFWCharCallback(GLFWwindow* window, unsigned int keycode);
void minimalGLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void minimalGLFWCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void minimalGLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

static void minimalGetGLVersion(const char* version_str, int* major, int* minor)
{
    const char* sep = ".";
    const char* major_str = version_str;
    const char* minor_str = version_str + strcspn(version_str, sep) + 1;

    if (major_str && major) *major = atoi(major_str);
    if (minor_str && minor) *minor = atoi(minor_str);
}

static int minimalInitGlfw(MinimalApp* app, const char* title, uint32_t w, uint32_t h, const char* gl_version)
{
    if (!glfwInit()) return MINIMAL_FAIL;

    int gl_major, gl_minor;
    minimalGetGLVersion(gl_version, &gl_major, &gl_minor);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    glfwSetErrorCallback(minimalGLFWErrorCallback);

    /* creating the window */
    app->window = glfwCreateWindow(w, h, title, NULL, NULL);
    if (!app->window) return MINIMAL_FAIL;

    glfwMakeContextCurrent(app->window);
    glfwSetWindowUserPointer(app->window, app);

    /* set GLFW callbacks */
    glfwSetWindowSizeCallback(app->window,  minimalGLFWWindowSizeCallback);
    glfwSetKeyCallback(app->window,         minimalGLFWKeyCallback);
    glfwSetCharCallback(app->window,        minimalGLFWCharCallback);
    glfwSetMouseButtonCallback(app->window, minimalGLFWMouseButtonCallback);
    glfwSetCursorPosCallback(app->window,   minimalGLFWCursorPosCallback);
    glfwSetScrollCallback(app->window,      minimalGLFWScrollCallback);

    return MINIMAL_OK;
}

int minimalLoad(MinimalApp* app, const char* title, uint32_t w, uint32_t h, const char* gl_version)
{
    if (minimalInitGlfw(app, title, w, h, gl_version) != MINIMAL_OK)
    {
        MINIMAL_ERROR("[GLFW] Failed to initialize GLFW.");
        glfwTerminate();
        return MINIMAL_FAIL;
    }

    /* apply settings */
    minimalEnableDebug(app, app->debug);
    minimalEnableVsync(app, app->vsync);

    minimalTimerReset(&app->timer);

    return (app->on_load) ? app->on_load(app, w, h) : MINIMAL_OK;
}

void minimalDestroy(MinimalApp* app)
{
    if (app->on_destroy) app->on_destroy(app);
    glfwDestroyWindow(app->window);
    glfwTerminate();
}

void minimalRun(MinimalApp* app)
{
    MINIMAL_ASSERT(app, "");
    MINIMAL_ASSERT(app->on_update, "Update callback missing!");

    while (!glfwWindowShouldClose(app->window)) {
        minimalTimerStart(&app->timer, glfwGetTime());
        minimalUpdateInput(app->window);

        app->on_update(app, (float)app->timer.deltatime);

        glfwPollEvents();
        glfwSwapBuffers(app->window);

        minimalTimerEnd(&app->timer, glfwGetTime());
    }
}

void minimalClose(MinimalApp* app) { glfwSetWindowShouldClose(app->window, GLFW_TRUE); }

/* --------------------------| settings |-------------------------------- */
void minimalSetWindowTitle(MinimalApp* app, const char* title) { glfwSetWindowTitle(app->window, title); }

void minimalEnableDebug(MinimalApp* app, uint8_t b) { app->debug = b; }
void minimalEnableVsync(MinimalApp* app, uint8_t b) { glfwSwapInterval(b); app->vsync = b; }

void minimalToggleDebug(MinimalApp* app) { minimalEnableDebug(app, !app->debug); }
void minimalToggleVsync(MinimalApp* app) { minimalEnableVsync(app, !app->vsync); }

/* --------------------------| glfw events |----------------------------- */
void minimalGLFWWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    MinimalApp* app = (MinimalApp*)glfwGetWindowUserPointer(window);
    if (app) minimalDispatchEvent(app, MINIMAL_EVENT_WINDOW_SIZE, 0, width, height);
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
