#include "input.h"

#include "platform.h"

typedef struct
{
    uint8_t state;
    uint8_t prev;
} InputState;

static InputState key_states[MINIMAL_KEY_LAST + 1];
static InputState mouse_states[MINIMAL_MOUSE_BUTTON_LAST + 1];

void minimalUpdateInput(MinimalWindow* context)
{
    for (int16_t i = MINIMAL_KEY_FIRST; i <= MINIMAL_KEY_LAST; ++i)
    {
        key_states[i].prev = key_states[i].state;
        key_states[i].state = (minimalGetKeyState(context, i) == MINIMAL_PRESS);
    }

    for (int16_t i = MINIMAL_MOUSE_BUTTON_1; i <= MINIMAL_MOUSE_BUTTON_LAST; ++i)
    {
        mouse_states[i].prev = mouse_states[i].state;
        mouse_states[i].state = (minimalGetMouseButtonState(context, i) == MINIMAL_PRESS);
    }
}

uint8_t minimalKeyPressed(int16_t keycode)
{
    if (!minimalKeycodeValid(keycode)) return 0;
    return key_states[keycode].state && !key_states[keycode].prev;
}

uint8_t minimalKeyReleased(int16_t keycode)
{
    if (!minimalKeycodeValid(keycode)) return 0;
    return key_states[keycode].prev && !key_states[keycode].state;
}

uint8_t minimalKeyDown(int16_t keycode)
{
    if (!minimalKeycodeValid(keycode)) return 0;
    return key_states[keycode].state;
}

uint8_t minimalKeyModActive(uint8_t mod)
{
    if (mod == 0) return 1;

    if ((mod & MINIMAL_KEY_MOD_SHIFT) 
        && !(key_states[MINIMAL_KEY_LEFT_SHIFT].state || key_states[MINIMAL_KEY_RIGHT_SHIFT].state))
        return 0;
    if ((mod & MINIMAL_KEY_MOD_CONTROL) 
        && !(key_states[MINIMAL_KEY_LEFT_CONTROL].state || key_states[MINIMAL_KEY_RIGHT_CONTROL].state))
        return 0;
    if ((mod & MINIMAL_KEY_MOD_ALT) 
        && !(key_states[MINIMAL_KEY_LEFT_ALT].state || key_states[MINIMAL_KEY_RIGHT_ALT].state))
        return 0;
    if ((mod & MINIMAL_KEY_MOD_SUPER) 
        && !(key_states[MINIMAL_KEY_LEFT_SUPER].state || key_states[MINIMAL_KEY_RIGHT_SUPER].state))
        return 0;

    return 1;
}

uint8_t minimalKeyPressedMod(int16_t keycode, uint8_t mod)
{
    return minimalKeyPressed(keycode) && minimalKeyModActive(mod);
}

uint8_t minimalKeyReleasedMod(int16_t keycode, uint8_t mod)
{
    return minimalKeyReleased(keycode) && minimalKeyModActive(mod);
}

uint8_t minimalKeyDownMod(int16_t keycode, uint8_t mod)
{
    return minimalKeyDown(keycode) && minimalKeyModActive(mod);
}

uint8_t minimalMousePressed(int8_t button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return mouse_states[button].state && !mouse_states[button].prev;
}

uint8_t minimalMouseReleased(int8_t button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return mouse_states[button].prev && !mouse_states[button].state;
}

uint8_t minimalMouseDown(int8_t button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return mouse_states[button].state;
}

void minimalCursorPos(float* x, float* y)
{
    minimalGetCursorPos(minimalGetCurrentContext(), x, y);
}

float minimalCursorX()
{
    float x;
    minimalCursorPos(&x, NULL);
    return x;
}

float minimalCursorY()
{
    float y;
    minimalCursorPos(NULL, &y);
    return y;
}