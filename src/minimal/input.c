#include "input.h"

#include "platform.h"


void minimalUpdateInput(const MinimalWindow* context, MinimalInput* input)
{
    for (int16_t i = MINIMAL_KEY_FIRST; i <= MINIMAL_KEY_LAST; ++i)
    {
        input->keys[i].prev = input->keys[i].state;
        input->keys[i].state = minimalGetKeyState(context, i);
    }

    minimalGetCursorPos(context, &input->cursorX, &input->cursorY);

    for (int8_t i = MINIMAL_MOUSE_BUTTON_1; i <= MINIMAL_MOUSE_BUTTON_LAST; ++i)
    {
        input->buttons[i].prev = input->buttons[i].state;
        input->buttons[i].state = minimalGetMouseButtonState(context, i);
    }
}

uint8_t minimalKeyPressed(const MinimalInput* input, int16_t keycode)
{
    if (!minimalKeycodeValid(keycode)) return 0;
    return input->keys[keycode].state && !input->keys[keycode].prev;
}

uint8_t minimalKeyReleased(const MinimalInput* input, int16_t keycode)
{
    if (!minimalKeycodeValid(keycode)) return 0;
    return input->keys[keycode].prev && !input->keys[keycode].state;
}

uint8_t minimalKeyDown(const MinimalInput* input, int16_t keycode)
{
    if (!minimalKeycodeValid(keycode)) return 0;
    return input->keys[keycode].state;
}

uint8_t minimalKeyModActive(const MinimalInput* input, uint8_t mod)
{
    if (mod == 0) return 1;

    if ((mod & MINIMAL_KEY_MOD_SHIFT)
        && !(input->keys[MINIMAL_KEY_LEFT_SHIFT].state || input->keys[MINIMAL_KEY_RIGHT_SHIFT].state))
        return 0;
    if ((mod & MINIMAL_KEY_MOD_CONTROL)
        && !(input->keys[MINIMAL_KEY_LEFT_CONTROL].state || input->keys[MINIMAL_KEY_RIGHT_CONTROL].state))
        return 0;
    if ((mod & MINIMAL_KEY_MOD_ALT)
        && !(input->keys[MINIMAL_KEY_LEFT_ALT].state || input->keys[MINIMAL_KEY_RIGHT_ALT].state))
        return 0;
    if ((mod & MINIMAL_KEY_MOD_SUPER)
        && !(input->keys[MINIMAL_KEY_LEFT_SUPER].state || input->keys[MINIMAL_KEY_RIGHT_SUPER].state))
        return 0;

    return 1;
}

uint8_t minimalMousePressed(const MinimalInput* input, int8_t button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return input->buttons[button].state && !input->buttons[button].prev;
}

uint8_t minimalMouseReleased(const MinimalInput* input, int8_t button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return input->buttons[button].prev && !input->buttons[button].state;
}

uint8_t minimalMouseDown(const MinimalInput* input, int8_t button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return input->buttons[button].state;
}

void minimalCursorPos(const MinimalInput* input, float* x, float* y)
{
    if (!input) return;
    if (x) *x = input->cursorX;
    if (y) *y = input->cursorY;
}

float minimalCursorX(const MinimalInput* input) { return input->cursorX; }
float minimalCursorY(const MinimalInput* input) { return input->cursorY; }
