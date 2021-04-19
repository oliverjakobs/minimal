#ifndef MINIMAL_H
#define MINIMAL_H

#include "MinimalWindow.h"

uint8_t MinimalInit();

double MinimalGetTime();

void MinimalMakeContextCurrent(MinimalWindow* context);
MinimalWindow* MinimalGetCurrentContext();

/*
 * -----------------------------------------------------------------------------------
 *                               Minimal App
 * -----------------------------------------------------------------------------------
 */
typedef struct MinimalApp MinimalApp;

typedef uint8_t (*MinimalLoadCB)    (MinimalApp* app);
typedef void    (*MinimalDestroyCB) (MinimalApp* app);

typedef void (*MinimalUpdateCB)     (MinimalApp* app, float deltatime);
typedef void (*MinimalRenderCB)     (MinimalApp* app);
typedef void (*MinimalRenderDebugCB)(MinimalApp* app);
typedef void (*MinimalRenderGUICB)  (MinimalApp* app);

struct MinimalApp
{
    char* title;
    MinimalWindow window;

    MinimalLoadCB       on_load;
    MinimalDestroyCB    on_destroy;

    MinimalUpdateCB         on_update;
    MinimalRenderCB         on_render;
    MinimalRenderDebugCB    on_render_debug;
    MinimalRenderGUICB      on_render_gui;

    uint8_t running;
    uint8_t debug;
    uint8_t vsync;

    MinimalTimer timer;
};

/* --------------------------| Setup |----------------------------------- */
uint8_t MinimalLoad(MinimalApp* app, const char* title, int width, int height, char* gl_version);
void MinimalDestroy(MinimalApp* app);

/* --------------------------| Game Loop |------------------------------- */
void MinimalRun(MinimalApp* app, void(*clear_buffer)());
void MinimalClose(MinimalApp* app);

void MinimalSetLoadCallback(MinimalApp* app, MinimalLoadCB callback);
void MinimalSetDestroyCallback(MinimalApp* app, MinimalDestroyCB callback);
void MinimalSetUpdateCallback(MinimalApp* app, MinimalUpdateCB callback);
void MinimalSetRenderCallback(MinimalApp* app, MinimalRenderCB callback);
void MinimalSetRenderDebugCallback(MinimalApp* app, MinimalRenderDebugCB callback);
void MinimalSetRenderGUICallback(MinimalApp* app, MinimalRenderGUICB callback);

/* --------------------------| Settings |-------------------------------- */
void MinimalEnableDebug(MinimalApp* app, int b);
void MinimalEnableVsync(MinimalApp* app, int b);

void MinimalToggleDebug(MinimalApp* app);
void MinimalToggleVsync(MinimalApp* app);

uint32_t MinimalGetFps(MinimalApp* app);

#endif // !MINIMAL_H
