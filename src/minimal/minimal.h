#ifndef MINIMAL_H
#define MINIMAL_H

#include "common.h"
#include "platform.h"
#include "event.h"
#include "input.h"

/* minimal version numbers */
#define MINIMAL_VERSION_MAJOR       1
#define MINIMAL_VERSION_MINOR       0
#define MINIMAL_VERSION_REVISION    0

void minimalGetVersion(int* major, int* minor, int* rev);
const char* minimalGetVersionString();

int  minimalInit();
void minimalTerminate();

/* --------------------------| minimal app |----------------------------- */
typedef int  (*MinimalLoadCB)    (MinimalApp* app, uint32_t w, uint32_t h);
typedef void (*MinimalDestroyCB) (MinimalApp* app);

typedef int  (*MinimalEventCB)   (MinimalApp* app, const MinimalEvent* e);
typedef void (*MinimalTickCB)    (MinimalApp* app, float deltatime);

struct MinimalApp
{
    MinimalWindow* window;
    MinimalInput input;

    MinimalLoadCB    on_load;
    MinimalDestroyCB on_destroy;

    MinimalEventCB on_event;
    MinimalTickCB  on_tick;

    uint32_t fps;

    // settings
    uint8_t debug : 1;
    uint8_t vsync : 1;
};

int  minimalLoad(MinimalApp* app, const char* title, uint32_t w, uint32_t h, const char* gl_version);
void minimalDestroy(MinimalApp* app);

/* --------------------------| game loop |------------------------------- */
void minimalRun(MinimalApp* app);
void minimalClose(MinimalApp* app);

/* --------------------------| settings |-------------------------------- */
void minimalSetTitle(MinimalApp* app, const char* title);

void minimalEnableDebug(MinimalApp* app, uint8_t  b);
void minimalEnableVsync(MinimalApp* app, uint8_t  b);

void minimalToggleDebug(MinimalApp* app);
void minimalToggleVsync(MinimalApp* app);

#endif // !MINIMAL_H
