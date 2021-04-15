#include "MinimalInput.h"

#include "MinimalWindow.h"

static int minimal_key_table[512];

void MinimalCreateKeyTable()
{
    memset(minimal_key_table, MINIMAL_KEY_UNKNOWN, sizeof(minimal_key_table));

    minimal_key_table[0x00B] = MINIMAL_KEY_0;
    minimal_key_table[0x002] = MINIMAL_KEY_1;
    minimal_key_table[0x003] = MINIMAL_KEY_2;
    minimal_key_table[0x004] = MINIMAL_KEY_3;
    minimal_key_table[0x005] = MINIMAL_KEY_4;
    minimal_key_table[0x006] = MINIMAL_KEY_5;
    minimal_key_table[0x007] = MINIMAL_KEY_6;
    minimal_key_table[0x008] = MINIMAL_KEY_7;
    minimal_key_table[0x009] = MINIMAL_KEY_8;
    minimal_key_table[0x00A] = MINIMAL_KEY_9;
    minimal_key_table[0x01E] = MINIMAL_KEY_A;
    minimal_key_table[0x030] = MINIMAL_KEY_B;
    minimal_key_table[0x02E] = MINIMAL_KEY_C;
    minimal_key_table[0x020] = MINIMAL_KEY_D;
    minimal_key_table[0x012] = MINIMAL_KEY_E;
    minimal_key_table[0x021] = MINIMAL_KEY_F;
    minimal_key_table[0x022] = MINIMAL_KEY_G;
    minimal_key_table[0x023] = MINIMAL_KEY_H;
    minimal_key_table[0x017] = MINIMAL_KEY_I;
    minimal_key_table[0x024] = MINIMAL_KEY_J;
    minimal_key_table[0x025] = MINIMAL_KEY_K;
    minimal_key_table[0x026] = MINIMAL_KEY_L;
    minimal_key_table[0x032] = MINIMAL_KEY_M;
    minimal_key_table[0x031] = MINIMAL_KEY_N;
    minimal_key_table[0x018] = MINIMAL_KEY_O;
    minimal_key_table[0x019] = MINIMAL_KEY_P;
    minimal_key_table[0x010] = MINIMAL_KEY_Q;
    minimal_key_table[0x013] = MINIMAL_KEY_R;
    minimal_key_table[0x01F] = MINIMAL_KEY_S;
    minimal_key_table[0x014] = MINIMAL_KEY_T;
    minimal_key_table[0x016] = MINIMAL_KEY_U;
    minimal_key_table[0x02F] = MINIMAL_KEY_V;
    minimal_key_table[0x011] = MINIMAL_KEY_W;
    minimal_key_table[0x02D] = MINIMAL_KEY_X;
    minimal_key_table[0x015] = MINIMAL_KEY_Y;
    minimal_key_table[0x02C] = MINIMAL_KEY_Z;

    minimal_key_table[0x028] = MINIMAL_KEY_APOSTROPHE;
    minimal_key_table[0x02B] = MINIMAL_KEY_BACKSLASH;
    minimal_key_table[0x033] = MINIMAL_KEY_COMMA;
    minimal_key_table[0x00D] = MINIMAL_KEY_EQUAL;
    minimal_key_table[0x029] = MINIMAL_KEY_GRAVE_ACCENT;
    minimal_key_table[0x01A] = MINIMAL_KEY_LEFT_BRACKET;
    minimal_key_table[0x00C] = MINIMAL_KEY_MINUS;
    minimal_key_table[0x034] = MINIMAL_KEY_PERIOD;
    minimal_key_table[0x01B] = MINIMAL_KEY_RIGHT_BRACKET;
    minimal_key_table[0x027] = MINIMAL_KEY_SEMICOLON;
    minimal_key_table[0x035] = MINIMAL_KEY_SLASH;

    minimal_key_table[0x00E] = MINIMAL_KEY_BACKSPACE;
    minimal_key_table[0x153] = MINIMAL_KEY_DELETE;
    minimal_key_table[0x14F] = MINIMAL_KEY_END;
    minimal_key_table[0x01C] = MINIMAL_KEY_ENTER;
    minimal_key_table[0x001] = MINIMAL_KEY_ESCAPE;
    minimal_key_table[0x147] = MINIMAL_KEY_HOME;
    minimal_key_table[0x152] = MINIMAL_KEY_INSERT;
    minimal_key_table[0x151] = MINIMAL_KEY_PAGE_DOWN;
    minimal_key_table[0x149] = MINIMAL_KEY_PAGE_UP;
    minimal_key_table[0x045] = MINIMAL_KEY_PAUSE;
    minimal_key_table[0x146] = MINIMAL_KEY_PAUSE;
    minimal_key_table[0x039] = MINIMAL_KEY_SPACE;
    minimal_key_table[0x00F] = MINIMAL_KEY_TAB;
    minimal_key_table[0x03A] = MINIMAL_KEY_CAPS_LOCK;
    minimal_key_table[0x145] = MINIMAL_KEY_NUM_LOCK;
    minimal_key_table[0x046] = MINIMAL_KEY_SCROLL_LOCK;
    minimal_key_table[0x03B] = MINIMAL_KEY_F1;
    minimal_key_table[0x03C] = MINIMAL_KEY_F2;
    minimal_key_table[0x03D] = MINIMAL_KEY_F3;
    minimal_key_table[0x03E] = MINIMAL_KEY_F4;
    minimal_key_table[0x03F] = MINIMAL_KEY_F5;
    minimal_key_table[0x040] = MINIMAL_KEY_F6;
    minimal_key_table[0x041] = MINIMAL_KEY_F7;
    minimal_key_table[0x042] = MINIMAL_KEY_F8;
    minimal_key_table[0x043] = MINIMAL_KEY_F9;
    minimal_key_table[0x044] = MINIMAL_KEY_F10;
    minimal_key_table[0x057] = MINIMAL_KEY_F11;
    minimal_key_table[0x058] = MINIMAL_KEY_F12;
    minimal_key_table[0x038] = MINIMAL_KEY_LEFT_ALT;
    minimal_key_table[0x01D] = MINIMAL_KEY_LEFT_CONTROL;
    minimal_key_table[0x02A] = MINIMAL_KEY_LEFT_SHIFT;
    minimal_key_table[0x15B] = MINIMAL_KEY_LEFT_SUPER;
    minimal_key_table[0x137] = MINIMAL_KEY_PRINT_SCREEN;
    minimal_key_table[0x138] = MINIMAL_KEY_RIGHT_ALT;
    minimal_key_table[0x11D] = MINIMAL_KEY_RIGHT_CONTROL;
    minimal_key_table[0x036] = MINIMAL_KEY_RIGHT_SHIFT;
    minimal_key_table[0x15C] = MINIMAL_KEY_RIGHT_SUPER;
    minimal_key_table[0x150] = MINIMAL_KEY_DOWN;
    minimal_key_table[0x14B] = MINIMAL_KEY_LEFT;
    minimal_key_table[0x14D] = MINIMAL_KEY_RIGHT;
    minimal_key_table[0x148] = MINIMAL_KEY_UP;

    minimal_key_table[0x052] = MINIMAL_KEY_NP_0;
    minimal_key_table[0x04F] = MINIMAL_KEY_NP_1;
    minimal_key_table[0x050] = MINIMAL_KEY_NP_2;
    minimal_key_table[0x051] = MINIMAL_KEY_NP_3;
    minimal_key_table[0x04B] = MINIMAL_KEY_NP_4;
    minimal_key_table[0x04C] = MINIMAL_KEY_NP_5;
    minimal_key_table[0x04D] = MINIMAL_KEY_NP_6;
    minimal_key_table[0x047] = MINIMAL_KEY_NP_7;
    minimal_key_table[0x048] = MINIMAL_KEY_NP_8;
    minimal_key_table[0x049] = MINIMAL_KEY_NP_9;
    minimal_key_table[0x04E] = MINIMAL_KEY_NP_ADD;
    minimal_key_table[0x053] = MINIMAL_KEY_NP_DECIMAL;
    minimal_key_table[0x135] = MINIMAL_KEY_NP_DIVIDE;
    minimal_key_table[0x11C] = MINIMAL_KEY_NP_ENTER;
    minimal_key_table[0x059] = MINIMAL_KEY_NP_EQUAL;
    minimal_key_table[0x037] = MINIMAL_KEY_NP_MULTIPLY;
    minimal_key_table[0x04A] = MINIMAL_KEY_NP_SUBTRACT;
}

int MinimalTranslateKey(unsigned int scancode)
{
    return minimal_key_table[scancode];
}

int MinimalKeycodeValid(unsigned int keycode)
{
    return keycode >= MINIMAL_KEY_FIRST && keycode <= MINIMAL_KEY_LAST;
}

int MinimalKeyPressed(MinimalWindow* window, unsigned int keycode)
{
    if (!MinimalKeycodeValid(keycode)) return 0;

    MinimalInputAction state = window->key_state[keycode];
    return state == MINIMAL_PRESS || state == MINIMAL_REPEAT;
}

int MinimalKeyReleased(MinimalWindow* window, unsigned int keycode)
{
    if (!MinimalKeycodeValid(keycode)) return 0;
    return window->key_state[keycode] == MINIMAL_RELEASE;
}

void MinimalGetCursorPos(MinimalWindow* window, float* x, float* y)
{
    POINT pos;
    if (GetCursorPos(&pos))
    {
        ScreenToClient(window->handle, &pos);
        if (x) *x = (float)pos.x;
        if (y) *y = (float)pos.y;
    }
    else
    {
        if (x) *x = 0.0f;
        if (y) *y = 0.0f;
    }
}
