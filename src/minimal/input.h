#ifndef MINIMAL_INPUT_H
#define MINIMAL_INPUT_H

#include "common.h"

typedef struct
{
    uint8_t state;
    uint8_t prev;
} MinimalInputState;

typedef struct
{
    MinimalInputState keys[MINIMAL_KEY_LAST + 1];
    MinimalInputState buttons[MINIMAL_MOUSE_BUTTON_LAST + 1];

    float cursorX, cursorY;
} MinimalInput;

void minimalUpdateInput(const MinimalWindow* context, MinimalInput* input);

uint8_t minimalKeyPressed(const MinimalInput* input, int16_t keycode);
uint8_t minimalKeyReleased(const MinimalInput* input, int16_t keycode);
uint8_t minimalKeyDown(const MinimalInput* input, int16_t keycode);

uint8_t minimalKeyModActive(const MinimalInput* input, uint8_t mod);

uint8_t minimalMousePressed(const MinimalInput* input, int8_t button);
uint8_t minimalMouseReleased(const MinimalInput* input, int8_t button);
uint8_t minimalMouseDown(const MinimalInput* input, int8_t button);

void minimalCursorPos(const MinimalInput* input, float* x, float* y);
float minimalCursorX(const MinimalInput* input);
float minimalCursorY(const MinimalInput* input);

#endif // !MINIMAL_INPUT_H