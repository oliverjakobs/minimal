#include "Input.h"

typedef struct
{
    uint8_t state;
    uint8_t prev;
} InputState;

static InputState key_states[GLFW_KEY_LAST + 1];
static InputState mouse_states[GLFW_MOUSE_BUTTON_LAST + 1];

void minimalUpdateInput(GLFWwindow* context)
{
    for (int16_t i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i)
    {
        key_states[i].prev = key_states[i].state;
        key_states[i].state = (glfwGetKey(context, i) == GLFW_PRESS);
    }

    for (int16_t i = GLFW_MOUSE_BUTTON_1; i <= GLFW_MOUSE_BUTTON_LAST; ++i)
    {
        mouse_states[i].prev = mouse_states[i].state;
        mouse_states[i].state = (glfwGetMouseButton(context, i) == GLFW_PRESS);
    }
}

uint8_t minimalKeyPressed(int16_t keycode)
{
    if (keycode > GLFW_KEY_LAST || keycode == GLFW_KEY_UNKNOWN) return 0;

    int state = glfwGetKey(glfwGetCurrentContext(), keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

uint8_t minimalKeyReleased(int16_t keycode)
{
    if (keycode > GLFW_KEY_LAST || keycode == GLFW_KEY_UNKNOWN) return 0;
    return glfwGetKey(glfwGetCurrentContext(), keycode) == GLFW_RELEASE;
}

uint8_t minimalKeyHit(int16_t keycode)
{
    if (keycode > GLFW_KEY_LAST || keycode <= GLFW_KEY_UNKNOWN) return 0;
    return key_states[keycode].state && !key_states[keycode].prev;
}

uint8_t minimalKeyDown(int16_t keycode)
{
    if (keycode > GLFW_KEY_LAST || keycode <= GLFW_KEY_UNKNOWN) return 0;
    return key_states[keycode].state;
}

uint8_t minimalKeyUp(int16_t keycode)
{
    if (keycode > GLFW_KEY_LAST || keycode <= GLFW_KEY_UNKNOWN) return 0;
    return key_states[keycode].prev && !key_states[keycode].state;
}

uint8_t minimalMousePressed(int8_t button)
{
    if (button > GLFW_MOUSE_BUTTON_LAST || button < GLFW_MOUSE_BUTTON_1) return 0;
    return glfwGetMouseButton(glfwGetCurrentContext(), button) == GLFW_PRESS;
}

uint8_t minimalMouseReleased(int8_t button)
{
    if (button > GLFW_MOUSE_BUTTON_LAST || button < GLFW_MOUSE_BUTTON_1) return 0;
    return glfwGetMouseButton(glfwGetCurrentContext(), button) == GLFW_RELEASE;
}

uint8_t minimalMouseHit(int8_t button)
{
    if (button > GLFW_MOUSE_BUTTON_LAST || button < GLFW_MOUSE_BUTTON_1) return 0;
    return mouse_states[button].state && !mouse_states[button].prev;
}

uint8_t minimalMouseDown(int8_t button)
{
    if (button > GLFW_MOUSE_BUTTON_LAST || button < GLFW_MOUSE_BUTTON_1) return 0;
    return mouse_states[button].state;
}

uint8_t minimalMouseUp(int8_t button)
{
    if (button > GLFW_MOUSE_BUTTON_LAST || button < GLFW_MOUSE_BUTTON_1) return 0;
    return mouse_states[button].prev && !mouse_states[button].state;
}

void minimalCursorPos(float* x, float* y)
{
    double xpos, ypos;
    glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);

    if (x) *x = (float)xpos;
    if (y) *y = (float)ypos;
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