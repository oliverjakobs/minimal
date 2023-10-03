#include "input.h"

#include "platform.h"

typedef struct
{
    uint8_t state;
    uint8_t prev;
} InputState;

typedef struct
{
    uint8_t state;
    uint8_t prev;
    float clickX, clickY;
} ButtonState;

typedef struct
{
    float posX, posY;
    float prevX, prevY;
    float deltaX, deltaY;
    float scrollX, scrollY;

    ButtonState button[MINIMAL_MOUSE_BUTTON_LAST + 1];
} MouseState;

static InputState key_states[MINIMAL_KEY_LAST + 1];
static MouseState mouse_state;

static void minimalUpdateButton(uint8_t index, int8_t state)
{
    ButtonState* button = &mouse_state.button[index];

    button->prev = button->state;
    button->state = state;

    if (button->prev == button->state) return;

    if (state == MINIMAL_PRESS)
    {
        button->clickX = mouse_state.posX;
        button->clickY = mouse_state.posY;
    }
}

void minimalUpdateInput(MinimalWindow* context)
{
    for (int16_t i = MINIMAL_KEY_FIRST; i <= MINIMAL_KEY_LAST; ++i)
    {
        key_states[i].prev = key_states[i].state;
        key_states[i].state = minimalGetKeyState(context, i);
    }

    // update mouse motion
    mouse_state.prevX = mouse_state.posX;
    mouse_state.prevY = mouse_state.posY;
    minimalGetCursorPos(context, &mouse_state.posX, &mouse_state.posY);
    mouse_state.deltaX = mouse_state.posX - mouse_state.prevX;
    mouse_state.deltaY = mouse_state.posY - mouse_state.prevY;

    // update mouse buttons
    for (int8_t i = MINIMAL_MOUSE_BUTTON_1; i <= MINIMAL_MOUSE_BUTTON_LAST; ++i)
    {
        minimalUpdateButton(i, minimalGetMouseButtonState(context, i));
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
    return mouse_state.button[button].state && !mouse_state.button[button].prev;
}

uint8_t minimalMouseReleased(int8_t button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return mouse_state.button[button].prev && !mouse_state.button[button].state;
}

uint8_t minimalMouseDown(int8_t button)
{
    if (!minimalMouseButtonValid(button)) return 0;
    return mouse_state.button[button].state;
}

void minimalCursorPos(float* x, float* y)
{
    if (x) *x = mouse_state.posX;
    if (y) *y = mouse_state.posY;
}

float minimalCursorX()
{
    return mouse_state.posX;
}

float minimalCursorY()
{
    return mouse_state.posY;
}