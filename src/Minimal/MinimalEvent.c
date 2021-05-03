#include "MinimalEvent.h"


MinimalBool MinimalCheckEventType(const MinimalEvent* e, uint32_t type)
{
    return e->type == type;
}

int32_t MinimalEventMouseButton(const MinimalEvent* e, float* x, float* y)
{
    if (e->type == MINIMAL_EVENT_MOUSE_BUTTON)
    {
        if (x) *x = (float)e->lParam;
        if (y) *y = (float)e->rParam;

        return MINIMAL_HIWORD(e->uParam);
    }
    
    return MINIMAL_MOUSE_BUTTON_UNKNOWN;
}

int32_t MinimalEventMouseButtonPressed(const MinimalEvent* e, float* x, float* y)
{
    int32_t buttoncode = MinimalEventMouseButton(e, x, y);
    return (MINIMAL_LOWORD(e->uParam) == MINIMAL_PRESS) ? buttoncode : MINIMAL_MOUSE_BUTTON_UNKNOWN;
}

int32_t MinimalEventMouseButtonReleased(const MinimalEvent* e, float* x, float* y)
{
    int32_t buttoncode = MinimalEventMouseButton(e, x, y);
    return (MINIMAL_LOWORD(e->uParam) == MINIMAL_RELEASE) ? buttoncode : MINIMAL_MOUSE_BUTTON_UNKNOWN;
}

MinimalBool MinimalEventMouseMoved(const MinimalEvent* e, float* x, float* y)
{
    if (e->type != MINIMAL_EVENT_MOUSE_MOVED) return MINIMAL_FALSE;

    if (x) *x = (float)e->lParam;
    if (y) *y = (float)e->rParam;

    return MINIMAL_TRUE;
}

int32_t MinimalEventKey(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_KEY) ? e->uParam : MINIMAL_KEY_UNKNOWN;
}

int32_t MinimalEventKeyPressed(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_KEY && e->lParam == MINIMAL_PRESS) ? e->uParam : MINIMAL_KEY_UNKNOWN;
}

int32_t MinimalEventKeyReleased(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_KEY && e->lParam == MINIMAL_RELEASE) ? e->uParam : MINIMAL_KEY_UNKNOWN;
}

char MinimalEventChar(const MinimalEvent* e)
{
    return (e->type == MINIMAL_EVENT_CHAR) ? (char)e->uParam : '\0';
}
