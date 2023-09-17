#include "event.h"

#include "minimal.h"

#define MINIMAL_LOWORD(dw) ((uint16_t)(dw))
#define MINIMAL_HIWORD(dw) ((uint16_t)(((uint32_t)(dw)) >> 16))

struct MinimalEvent
{
    uint32_t type;
    uint32_t uParam;
    int32_t lParam;
    int32_t rParam;
    void* user_data;
};

void minimalDispatchEvent(MinimalApp* app, uint32_t type, uint32_t uParam, int32_t lParam, int32_t rParam)
{
    MinimalEvent e = { .type = type, .uParam = uParam, .lParam = lParam, .rParam = rParam, .user_data = NULL };
    if (app && app->on_event) app->on_event(app, &e);
}

void minimalDispatchExternalEvent(MinimalApp* app, uint32_t type, void* data)
{
    MinimalEvent e = { .type = MINIMAL_EVENT_EXTERNAL, .uParam = type, .user_data = data };
    if (app && app->on_event) app->on_event(app, &e);
}

uint32_t minimalEventExternal(const MinimalEvent* e)
{
    if (e->type != MINIMAL_EVENT_EXTERNAL) return 0;
    return e->uParam; // return external event id
}

void* minimalExternalEventData(const MinimalEvent* e)
{
    if (e->type != MINIMAL_EVENT_EXTERNAL) return NULL;
    return e->user_data;
}

uint8_t minimalCheckEventType(const MinimalEvent* e, uint32_t type) { return e->type == type; }

uint8_t minimalEventWindowSize(const MinimalEvent* e, float* w, float* h)
{
    if (e->type != MINIMAL_EVENT_WINDOW_SIZE) return 0;

    if (w) *w = (float)e->lParam;
    if (h) *h = (float)e->rParam;

    return 1;
}

int32_t minimalEventMouseButton(const MinimalEvent* e, float* x, float* y)
{
    if (e->type != MINIMAL_EVENT_MOUSE_BUTTON) return MINIMAL_MOUSE_BUTTON_UNKNOWN;

    if (x) *x = (float)e->lParam;
    if (y) *y = (float)e->rParam;

    return MINIMAL_HIWORD(e->uParam);
}

int32_t minimalEventMouseButtonPressed(const MinimalEvent* e, float* x, float* y)
{
    int32_t buttoncode = minimalEventMouseButton(e, x, y);
    return (MINIMAL_LOWORD(e->uParam) == MINIMAL_PRESS) ? buttoncode : MINIMAL_MOUSE_BUTTON_UNKNOWN;
}

int32_t minimalEventMouseButtonReleased(const MinimalEvent* e, float* x, float* y)
{
    int32_t buttoncode = minimalEventMouseButton(e, x, y);
    return (MINIMAL_LOWORD(e->uParam) == MINIMAL_RELEASE) ? buttoncode : MINIMAL_MOUSE_BUTTON_UNKNOWN;
}

uint8_t minimalEventMouseMoved(const MinimalEvent* e, float* x, float* y)
{
    if (e->type != MINIMAL_EVENT_MOUSE_MOVED) return 0;

    if (x) *x = (float)e->lParam;
    if (y) *y = (float)e->rParam;

    return 1;
}

int32_t minimalEventKey(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_KEY) ? e->uParam : MINIMAL_KEY_UNKNOWN;
}

int32_t minimalEventKeyPressed(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_KEY && e->lParam == MINIMAL_PRESS) ? e->uParam : MINIMAL_KEY_UNKNOWN;
}

int32_t minimalEventKeyReleased(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_KEY && e->lParam == MINIMAL_RELEASE) ? e->uParam : MINIMAL_KEY_UNKNOWN;
}

char minimalEventChar(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_CHAR) ? (char)e->uParam : '\0';
}
