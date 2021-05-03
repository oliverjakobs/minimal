#ifndef MINIMAL_H
#define MINIMAL_H

#include "MinimalWindow.h"
#include "MinimalEvent.h"

/* minimal version numbers */
#define MINIMAL_VERSION_MAJOR       1
#define MINIMAL_VERSION_MINOR       3
#define MINIMAL_VERSION_REVISION    1

MinimalBool MinimalInit();
void MinimalTerminate();

double MinimalGetTime();

void MinimalMakeContextCurrent(MinimalWindow* context);
MinimalWindow* MinimalGetCurrentContext();

void MinimalGetVersion(int* major, int* minor, int* rev);
const char* MinimalGetVersionString();

/* --------------------------| minimal app |----------------------------- */
typedef struct MinimalApp MinimalApp;

typedef MinimalBool (*MinimalLoadCB)    (MinimalApp* app, uint32_t w, uint32_t h);
typedef void        (*MinimalDestroyCB) (MinimalApp* app);

typedef MinimalBool (*MinimalEventCB)   (MinimalApp* app, const MinimalEvent* e);
typedef void        (*MinimalUpdateCB)  (MinimalApp* app, float deltatime);
typedef void        (*MinimalRenderCB)  (MinimalApp* app);

struct MinimalApp
{
    MinimalWindow* window;

    MinimalLoadCB    on_load;
    MinimalDestroyCB on_destroy;

    MinimalEventCB  on_event;
    MinimalUpdateCB on_update;
    MinimalRenderCB on_render;
    MinimalRenderCB on_render_debug;
    MinimalRenderCB on_render_gui;

    MinimalBool debug;
    MinimalBool vsync;

    MinimalTimer timer;
};

MinimalBool MinimalLoad(MinimalApp* app, const char* title, uint32_t w, uint32_t h, const char* gl);
void MinimalDestroy(MinimalApp* app);

/* --------------------------| game loop |------------------------------- */
void MinimalRun(MinimalApp* app, void(*clear_buffer)());
void MinimalClose(MinimalApp* app);

void MinimalSetLoadCallback(MinimalApp* app, MinimalLoadCB callback);
void MinimalSetDestroyCallback(MinimalApp* app, MinimalDestroyCB callback);
void MinimalSetEventCallback(MinimalApp* app, MinimalEventCB callback);
void MinimalSetUpdateCallback(MinimalApp* app, MinimalUpdateCB callback);
void MinimalSetRenderCallback(MinimalApp* app, MinimalRenderCB callback);
void MinimalSetRenderDebugCallback(MinimalApp* app, MinimalRenderCB callback);
void MinimalSetRenderGUICallback(MinimalApp* app, MinimalRenderCB callback);

/* --------------------------| settings |-------------------------------- */
void MinimalSetTitle(MinimalApp* app, const char* title);

void MinimalEnableDebug(MinimalApp* app, MinimalBool b);
void MinimalEnableVsync(MinimalApp* app, MinimalBool b);

void MinimalToggleDebug(MinimalApp* app);
void MinimalToggleVsync(MinimalApp* app);

uint32_t MinimalGetFps(const MinimalApp* app);

/* --------------------------| event |----------------------------------- */
void MinimalDispatchEvent(MinimalApp* app, uint32_t type, uint32_t uParam, int32_t lParam, int32_t rParam);

/* --------------------------| input |----------------------------------- */
MinimalBool MinimalKeyPressed(uint32_t keycode);
MinimalBool MinimalKeyReleased(uint32_t  keycode);
MinimalBool MinimalKeyDown(uint32_t  keycode);

MinimalBool MinimalMouseButtonPressed(uint32_t button);
MinimalBool MinimalMouseButtonReleased(uint32_t  button);

void MinimalGetCursorPos(float* x, float* y);

#endif // !MINIMAL_H
