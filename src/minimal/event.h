#ifndef MINIMAL_EVENT_H
#define MINIMAL_EVENT_H

#include "common.h"

#define MINIMAL_EVENT_UNKOWN            0

/* window events*/
#define MINIMAL_EVENT_WINDOW_SIZE       1
#define MINIMAL_EVENT_WINDOW_MINIMIZE   2
#define MINIMAL_EVENT_WINDOW_MAXIMIZE   3

/* key events */
#define MINIMAL_EVENT_KEY               10
#define MINIMAL_EVENT_CHAR              11

/* mouse events */
#define MINIMAL_EVENT_MOUSE_BUTTON      12
#define MINIMAL_EVENT_MOUSE_MOVED       13
#define MINIMAL_EVENT_MOUSE_SCROLLED    14

#define MINIMAL_EVENT_EXTERNAL          20

void minimalDispatchEvent(MinimalApp* app, uint32_t type, uint32_t uParam, int32_t lParam, int32_t rParam);
void minimalDispatchExternalEvent(MinimalApp* app, uint32_t type, void* data);

/* Utility */
uint32_t minimalEventExternal(const MinimalEvent* e);
void*    minimalExternalEventData(const MinimalEvent* e);

uint8_t minimalCheckEventType(const MinimalEvent* e, uint32_t type);

uint8_t minimalEventWindowSize(const MinimalEvent* e, float* w, float* h);

int32_t minimalEventMouseButton(const MinimalEvent* e, float* x, float* y);
int32_t minimalEventMouseButtonPressed(const MinimalEvent* e, float* x, float* y);
int32_t minimalEventMouseButtonReleased(const MinimalEvent* e, float* x, float* y);
uint8_t minimalEventMouseMoved(const MinimalEvent* e, float* x, float* y);

int32_t minimalEventKey(const MinimalEvent* e);
int32_t minimalEventKeyPressed(const MinimalEvent* e);
int32_t minimalEventKeyReleased(const MinimalEvent* e);

char minimalEventChar(const MinimalEvent* e);

#endif /* !MINIMAL_EVENT_H */
