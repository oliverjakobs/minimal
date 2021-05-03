#ifndef MINIMAL_EVENT_H
#define MINIMAL_EVENT_H

#include "MinimalUtil.h"

#define MINIMAL_LOWORD(dw) ((uint16_t)(dw))
#define MINIMAL_HIWORD(dw) ((uint16_t)(((uint32_t)(dw)) >> 16))

#define MINIMAL_MAKE_MOUSE_U(bc, a) ((bc << 16) + a)

typedef void(*MinimalDispatchEventCB)(void* handler, uint32_t type, uint32_t u, int32_t l, int32_t r);

#define MINIMAL_EVENT_UNKOWN            0

/* window events*/
#define MINIMAL_EVENT_WINDOW_CLOSE      1
#define MINIMAL_EVENT_WINDOW_SIZE       2
#define MINIMAL_EVENT_WINDOW_FOCUS      3
#define MINIMAL_EVENT_WINDOW_MOVED      4

/* key events */
#define MINIMAL_EVENT_KEY               5
#define MINIMAL_EVENT_CHAR              6

/* mouse events */
#define MINIMAL_EVENT_MOUSE_BUTTON      7
#define MINIMAL_EVENT_MOUSE_MOVED       8
#define MINIMAL_EVENT_MOUSE_SCROLLED    9

typedef struct
{
	uint32_t type;
    uint32_t uParam;
    int32_t lParam;
    int32_t rParam;
} MinimalEvent;

/* Utility */
MinimalBool MinimalCheckEventType(const MinimalEvent* e, uint32_t type);

int32_t MinimalEventMouseButton(const MinimalEvent* e, float* x, float* y);
int32_t MinimalEventMouseButtonPressed(const MinimalEvent* e, float* x, float* y);
int32_t MinimalEventMouseButtonReleased(const MinimalEvent* e, float* x, float* y);
MinimalBool MinimalEventMouseMoved(const MinimalEvent* e, float* x, float* y);

int32_t MinimalEventKey(const MinimalEvent* e);
int32_t MinimalEventKeyPressed(const MinimalEvent* e);
int32_t MinimalEventKeyReleased(const MinimalEvent* e);

char MinimalEventChar(const MinimalEvent* e);

#endif /* !MINIMAL_EVENT_H */
