#ifndef MINIMAL_APPLICATION_H
#define MINIMAL_APPLICATION_H

#include <GLFW/glfw3.h>

#include "Utils.h"
#include "Input.h"
#include "Event.h"

/* --------------------------| minimal app |----------------------------- */
typedef int  (*MinimalLoadCB)    (MinimalApp* app, uint32_t w, uint32_t h);
typedef void (*MinimalDestroyCB) (MinimalApp* app);

typedef int  (*MinimalEventCB)   (MinimalApp* app, const MinimalEvent* e);
typedef void (*MinimalUpdateCB)  (MinimalApp* app, float deltatime);

struct MinimalApp
{
    GLFWwindow* window;

    MinimalLoadCB    on_load;
    MinimalDestroyCB on_destroy;

    MinimalEventCB  on_event;
    MinimalUpdateCB on_update;

    MinimalTimer timer;

    // settings
    uint8_t debug : 1;
    uint8_t vsync : 1;
};

int  minimalLoad(MinimalApp* app, const char* title, uint32_t w, uint32_t h, int gl_major, int gl_minor);
void minimalDestroy(MinimalApp* app);

void minimalRun(MinimalApp* app);
void minimalClose(MinimalApp* app);

/* --------------------------| settings |-------------------------------- */
void minimalSetWindowTitle(MinimalApp* app, const char* title);

void minimalEnableDebug(MinimalApp* app, uint8_t b);
void minimalEnableVsync(MinimalApp* app, uint8_t b);

void minimalToggleDebug(MinimalApp* app);
void minimalToggleVsync(MinimalApp* app);

#endif // !MINIMAL_APPLICATION_H
