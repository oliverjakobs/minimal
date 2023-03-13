#ifndef MINIMAL_INPUT_H
#define MINIMAL_INPUT_H

#include <GLFW/glfw3.h>

void minimalUpdateInput(GLFWwindow* context);

uint8_t minimalKeyPressed(int16_t keycode);
uint8_t minimalKeyReleased(int16_t keycode);

uint8_t minimalKeyHit(int16_t keycode);
uint8_t minimalKeyDown(int16_t keycode);
uint8_t minimalKeyUp(int16_t keycode);

uint8_t minimalMousePressed(int8_t button);
uint8_t minimalMouseReleased(int8_t button);

uint8_t minimalMouseHit(int8_t button);
uint8_t minimalMouseDown(int8_t button);
uint8_t minimalMouseUp(int8_t button);

void minimalCursorPos(float* x, float* y);
float minimalCursorX();
float minimalCursorY();

#endif // !MINIMAL_INPUT_H
