#ifndef MINIMAL_INPUT_H
#define MINIMAL_INPUT_H

#include "common.h"

void minimalUpdateInput(MinimalWindow* context);

uint8_t minimalKeyPressed(int16_t keycode);
uint8_t minimalKeyReleased(int16_t keycode);
uint8_t minimalKeyDown(int16_t keycode);

uint8_t minimalKeyModActive(uint8_t mod);

uint8_t minimalMousePressed(int8_t button);
uint8_t minimalMouseReleased(int8_t button);
uint8_t minimalMouseDown(int8_t button);

void minimalCursorPos(float* x, float* y);
float minimalCursorX();
float minimalCursorY();

#endif // !MINIMAL_INPUT_H