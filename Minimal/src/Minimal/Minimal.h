#ifndef MINIMAL_H
#define MINIMAL_H

#include "MinimalWindow.h"

/* Minimal version numbers */
#define MINIMAL_VERSION_MAJOR       1
#define MINIMAL_VERSION_MINOR       0
#define MINIMAL_VERSION_REVISION    0

MinimalBool MinimalInit();
void MinimalTerminate();

double MinimalGetTime();

void MinimalMakeContextCurrent(MinimalWindow* context);
MinimalWindow* MinimalGetCurrentContext();

void MinimalGetVersion(int* major, int* minor, int* rev);
const char* MinimalGetVersionString(void);

/* --------------------------| Minimal App |----------------------------- */
typedef struct MinimalApp MinimalApp;

typedef MinimalBool (*MinimalLoadCB)    (MinimalApp* app);
typedef void        (*MinimalDestroyCB) (MinimalApp* app);

typedef void (*MinimalUpdateCB) (MinimalApp* app, float deltatime);
typedef void (*MinimalRenderCB) (MinimalApp* app);

struct MinimalApp
{
    MinimalWindow window;

    MinimalLoadCB       on_load;
    MinimalDestroyCB    on_destroy;

    MinimalUpdateCB on_update;
    MinimalRenderCB on_render;
    MinimalRenderCB on_render_debug;
    MinimalRenderCB on_render_gui;

    MinimalBool debug;
    MinimalBool vsync;

    MinimalTimer timer;
};

uint8_t MinimalLoad(MinimalApp* app, const char* title, int width, int height, char* gl_version);
void MinimalDestroy(MinimalApp* app);

/* --------------------------| Game Loop |------------------------------- */
void MinimalRun(MinimalApp* app, void(*clear_buffer)());
void MinimalClose(MinimalApp* app);

void MinimalSetLoadCallback(MinimalApp* app, MinimalLoadCB callback);
void MinimalSetDestroyCallback(MinimalApp* app, MinimalDestroyCB callback);
void MinimalSetUpdateCallback(MinimalApp* app, MinimalUpdateCB callback);
void MinimalSetRenderCallback(MinimalApp* app, MinimalRenderCB callback);
void MinimalSetRenderDebugCallback(MinimalApp* app, MinimalRenderCB callback);
void MinimalSetRenderGUICallback(MinimalApp* app, MinimalRenderCB callback);

/* --------------------------| Settings |-------------------------------- */
void MinimalEnableDebug(MinimalApp* app, MinimalBool b);
void MinimalEnableVsync(MinimalApp* app, MinimalBool b);

void MinimalToggleDebug(MinimalApp* app);
void MinimalToggleVsync(MinimalApp* app);

uint32_t MinimalGetFps(MinimalApp* app);

/* --------------------------| Input |----------------------------------- */
MinimalBool MinimalKeyPressed(uint32_t keycode);
MinimalBool MinimalKeyReleased(uint32_t  keycode);
MinimalBool MinimalKeyDown(uint32_t  keycode);

MinimalBool MinimalMouseButtonPressed(uint32_t button);
MinimalBool MinimalMouseButtonReleased(uint32_t  button);

void MinimalGetCursorPos(float* x, float* y);

#endif // !MINIMAL_H
