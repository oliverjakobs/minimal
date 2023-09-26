#include "event.h"

#include "minimal.h"

#define MINIMAL_LOWORD(dw) ((uint16_t)(dw))
#define MINIMAL_HIWORD(dw) ((uint16_t)(((uint32_t)(dw)) >> 16))

struct MinimalEvent
{
    uint32_t type;
    union
    {
        struct
        {
            uint32_t uParam;
            int32_t lParam;
            int32_t rParam;
        };
        const void* external;
    };
};

void minimalDispatchEvent(MinimalApp* app, uint32_t type, uint32_t uParam, int32_t lParam, int32_t rParam)
{
    MinimalEvent e = { .type = type, .uParam = uParam, .lParam = lParam, .rParam = rParam };
    if (app && app->on_event) app->on_event(app, &e);
}

void minimalDispatchExternalEvent(MinimalApp* app, uint32_t type, const void* data)
{
    MinimalEvent e = { .type = type, .external = data };
    if (app && app->on_event) app->on_event(app, &e);
}

uint8_t minimalEventIsType(const MinimalEvent* e, uint32_t type) { return e->type == type; }
uint8_t minimalEventIsExternal(const MinimalEvent* e)            { return e->type > MINIMAL_EVENT_LAST; }

const void* minimalExternalEvent(const MinimalEvent* e)
{
    return minimalEventIsExternal(e) ? e->external : NULL;
}

uint8_t minimalEventWindowSize(const MinimalEvent* e, float* w, float* h)
{
    if (!minimalEventIsType(e, MINIMAL_EVENT_WINDOW_SIZE)) return 0;

    if (w) *w = (float)e->lParam;
    if (h) *h = (float)e->rParam;

    return 1;
}

uint8_t minimalEventMouseButton(const MinimalEvent* e, int8_t button, float* x, float* y)
{
    if (!minimalEventIsType(e, MINIMAL_EVENT_MOUSE_BUTTON)) return 0;

    if (button != (int8_t)MINIMAL_HIWORD(e->uParam)) return 0;

    if (x) *x = (float)e->lParam;
    if (y) *y = (float)e->rParam;

    return 1;
}

uint8_t minimalEventMouseButtonPressed(const MinimalEvent* e, int8_t button, float* x, float* y)
{
    return minimalEventMouseButton(e, button, x, y) && MINIMAL_LOWORD(e->uParam) == MINIMAL_PRESS;
}

uint8_t minimalEventMouseButtonReleased(const MinimalEvent* e, int8_t button, float* x, float* y)
{
    return minimalEventMouseButton(e, button, x, y) && MINIMAL_LOWORD(e->uParam) == MINIMAL_RELEASE;
}

uint16_t minimalEventMouseButtonAction(const MinimalEvent* e)
{
    return MINIMAL_LOWORD(e->uParam);
}

uint8_t minimalEventMouseMoved(const MinimalEvent* e, float* x, float* y)
{
    if (!minimalEventIsType(e, MINIMAL_EVENT_MOUSE_MOVED)) return 0;

    if (x) *x = (float)e->lParam;
    if (y) *y = (float)e->rParam;

    return 1;
}

uint8_t minimalEventMouseScrolled(const MinimalEvent* e, float* xoffset, float* yoffset)
{
    if (!minimalEventIsType(e, MINIMAL_EVENT_MOUSE_SCROLLED)) return 0;

    if (xoffset) *xoffset = (float)e->lParam;
    if (yoffset) *yoffset = (float)e->rParam;

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
