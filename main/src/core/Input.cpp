

#include "Input.hpp"
#include "Utils.hpp"

Vector2 Mouse::currentPosition;
Vector2 Mouse::previousPosition;
char Mouse::currentButtonState[Mouse::MAX_BUTTONS];
char Mouse::previousButtonState[Mouse::MAX_BUTTONS];
Vector2 Mouse::currentWheelMove;
Vector2 Mouse::previousWheelMove;
Vector2 Mouse::offset;
Vector2 Mouse::scale;
bool Mouse::cursorRelative = false;

char Keyboard::currentKeyState[Keyboard::MAX_KEYS];
char Keyboard::previousKeyState[Keyboard::MAX_KEYS];
char Keyboard::keyRepeatInFrame[Keyboard::MAX_KEYS];
int Keyboard::charPressedQueue[Keyboard::MAX_KEY_QUEUE];
int Keyboard::keyPressedQueue[Keyboard::MAX_KEY_QUEUE];
int Keyboard::keyPressedQueueCount = 0;
int Keyboard::charPressedQueueCount = 0;

void Mouse::Update()
{
    if (cursorRelative)
        currentPosition=Vector2(0, 0);
    else
        previousPosition = currentPosition;
    for (int i = 0; i < MAX_BUTTONS; i++)
        previousButtonState[i] = previousButtonState[i];
}

void Mouse::Init()
{
    offset=Vector2(0, 0);
    scale=Vector2(1, 1);
    currentPosition=Vector2(0, 0);
    previousPosition=Vector2(0, 0);
    currentWheelMove=Vector2(0, 0);
    previousWheelMove=Vector2(0, 0);

    cursorRelative = false;
    for (int i = 0; i < MAX_BUTTONS; i++)
    {
        previousButtonState[i] = false;
        previousButtonState[i] = false;
    }
}

void Mouse::setMousePosition(int x, int y, int xrel, int yrel)
{
    if (cursorRelative)
    {
        currentPosition.x = (float)xrel;
        currentPosition.y = (float)yrel;
        previousPosition.x=0;
        previousPosition.y=0;
    }
    else
    {
        currentPosition.x = (float)x;
        currentPosition.y = (float)y;
    }
}

void Mouse::setMouseWheel(int x, int y)
{

    currentWheelMove.x = (float)x;
    currentWheelMove.y = (float)y;
}

void Mouse::setMouseButton(int button, bool state)
{
    currentButtonState[button] = state;
}

bool Mouse::Down(int button)
{
    bool down = false;

    if (currentButtonState[button] == 1)
        down = true;
    return down;
}

bool Mouse::Up(int button)
{
    bool up = false;

    if (currentButtonState[button] == 0)
        up = true;

    return up;
}

bool Mouse::Pressed(int button)
{
    bool pressed = false;

    if ((currentButtonState[button] == 1) && (previousButtonState[button] == 0))
        pressed = true;

    return pressed;
}

bool Mouse::Released(int button)
{
    bool released = false;

    if ((currentButtonState[button] == 0) && (previousButtonState[button] == 1))
        released = true;

    return released;
}

float Mouse::X()
{
    return (int)((currentPosition.x + offset.x) * scale.x);
}

float Mouse::Y()
{
    return (int)((currentPosition.y + offset.y) * scale.y);
}

float Mouse::dX()
{
    return currentPosition.x - previousPosition.x;
}

float Mouse::dY()
{
    return currentPosition.y - previousPosition.y;
}

float Mouse::Wheel()
{
    float result = 0.0f;

    if (fabsf(currentWheelMove.x) > fabsf(currentWheelMove.y))
        result = (float)currentWheelMove.x;
    else
        result = (float)currentWheelMove.y;

    return result;
}

Vector2 Mouse::Position()
{

    return currentPosition;
}

Vector2 Mouse::Delta()
{
    return currentPosition - previousPosition;
}

void Mouse::SetOffset(int offsetX, int offsetY)
{
    offset=Vector2(offsetX, offsetY);
}

void Mouse::SetScale(float scaleX, float scaleY)
{
    scale=Vector2(scaleX, scaleY);
}

bool Keyboard::Down(int key)
{
    bool down = false;

    if ((key > 0) && (key < MAX_KEYS))
    {
        if (currentKeyState[key] == 1)
            down = true;
    }

    return down;
}

bool Keyboard::Up(int key)
{
    bool up = false;

    if ((key > 0) && (key < MAX_KEYS))
    {
        if (currentKeyState[key] == 0)
            up = true;
    }

    return up;
}

bool Keyboard::Pressed(int key)
{

    bool pressed = false;

    if ((key > 0) && (key < MAX_KEYS))
    {
        if ((previousKeyState[key] == 0) && (currentKeyState[key] == 1))
            pressed = true;
    }

    return pressed;
}

bool Keyboard::Released(int key)
{
    bool released = false;

    if ((key > 0) && (key < MAX_KEYS))
    {
        if ((previousKeyState[key] == 1) && (currentKeyState[key] == 0))
            released = true;
    }

    return released;
}

bool Keyboard::PressedRepeat(int key)
{
    bool repeat = false;

    if ((key > 0) && (key < MAX_KEYS))
    {
        if (keyRepeatInFrame[key] == 1)
            repeat = true;
    }

    return repeat;
}

int Keyboard::getChar()
{
    int value = 0;

    if (charPressedQueueCount > 0)
    {
        value = charPressedQueue[0];
        for (int i = 0; i < (charPressedQueueCount - 1); i++)
            charPressedQueue[i] = charPressedQueue[i + 1];

        charPressedQueue[charPressedQueueCount - 1] = 0;
        charPressedQueueCount--;
    }

    return value;
}

int Keyboard::getKey()
{
    int value = 0;

    if (keyPressedQueueCount > 0)
    {
        value = keyPressedQueue[0];
        for (int i = 0; i < (keyPressedQueueCount - 1); i++)
            keyPressedQueue[i] = keyPressedQueue[i + 1];

        keyPressedQueue[keyPressedQueueCount - 1] = 0;
        keyPressedQueueCount--;
    }

    return value;
}

void Keyboard::Update()
{
    keyPressedQueueCount = 0;
    charPressedQueueCount = 0;

    for (int i = 0; i < MAX_KEYS; i++)
    {
        previousKeyState[i] = currentKeyState[i];
        keyRepeatInFrame[i] = 0;
    }
}

void Keyboard::Init()
{
    for (int i = 0; i < MAX_KEY_QUEUE; i++)
    {
        keyPressedQueue[i] = 0;
        charPressedQueue[i] = 0;
    }

    for (int i = 0; i < MAX_KEYS; i++)
    {
        previousKeyState[i] = false;
        currentKeyState[i] = false;
        keyRepeatInFrame[i] = false;
    }
}




#define SCANCODE_MAPPED_NUM 232
static const int ScancodeToKey[SCANCODE_MAPPED_NUM] =
{
    KEY_NULL,           // SDL_SCANCODE_UNKNOWN
    0,
    0,
    0,
    KEY_A,              // SDL_SCANCODE_A
    KEY_B,              // SDL_SCANCODE_B
    KEY_C,              // SDL_SCANCODE_C
    KEY_D,              // SDL_SCANCODE_D
    KEY_E,              // SDL_SCANCODE_E
    KEY_F,              // SDL_SCANCODE_F
    KEY_G,              // SDL_SCANCODE_G
    KEY_H,              // SDL_SCANCODE_H
    KEY_I,              // SDL_SCANCODE_I
    KEY_J,              // SDL_SCANCODE_J
    KEY_K,              // SDL_SCANCODE_K
    KEY_L,              // SDL_SCANCODE_L
    KEY_M,              // SDL_SCANCODE_M
    KEY_N,              // SDL_SCANCODE_N
    KEY_O,              // SDL_SCANCODE_O
    KEY_P,              // SDL_SCANCODE_P
    KEY_Q,              // SDL_SCANCODE_Q
    KEY_R,              // SDL_SCANCODE_R
    KEY_S,              // SDL_SCANCODE_S
    KEY_T,              // SDL_SCANCODE_T
    KEY_U,              // SDL_SCANCODE_U
    KEY_V,              // SDL_SCANCODE_V
    KEY_W,              // SDL_SCANCODE_W
    KEY_X,              // SDL_SCANCODE_X
    KEY_Y,              // SDL_SCANCODE_Y
    KEY_Z,              // SDL_SCANCODE_Z
    KEY_ONE,            // SDL_SCANCODE_1
    KEY_TWO,            // SDL_SCANCODE_2
    KEY_THREE,          // SDL_SCANCODE_3
    KEY_FOUR,           // SDL_SCANCODE_4
    KEY_FIVE,           // SDL_SCANCODE_5
    KEY_SIX,            // SDL_SCANCODE_6
    KEY_SEVEN,          // SDL_SCANCODE_7
    KEY_EIGHT,          // SDL_SCANCODE_8
    KEY_NINE,           // SDL_SCANCODE_9
    KEY_ZERO,           // SDL_SCANCODE_0
    KEY_ENTER,          // SDL_SCANCODE_RETURN
    KEY_ESCAPE,         // SDL_SCANCODE_ESCAPE
    KEY_BACKSPACE,      // SDL_SCANCODE_BACKSPACE
    KEY_TAB,            // SDL_SCANCODE_TAB
    KEY_SPACE,          // SDL_SCANCODE_SPACE
    KEY_MINUS,          // SDL_SCANCODE_MINUS
    KEY_EQUAL,          // SDL_SCANCODE_EQUALS
    KEY_LEFT_BRACKET,   // SDL_SCANCODE_LEFTBRACKET
    KEY_RIGHT_BRACKET,  // SDL_SCANCODE_RIGHTBRACKET
    KEY_BACKSLASH,      // SDL_SCANCODE_BACKSLASH
    0,                  // SDL_SCANCODE_NONUSHASH
    KEY_SEMICOLON,      // SDL_SCANCODE_SEMICOLON
    KEY_APOSTROPHE,     // SDL_SCANCODE_APOSTROPHE
    KEY_GRAVE,          // SDL_SCANCODE_GRAVE
    KEY_COMMA,          // SDL_SCANCODE_COMMA
    KEY_PERIOD,         // SDL_SCANCODE_PERIOD
    KEY_SLASH,          // SDL_SCANCODE_SLASH
    KEY_CAPS_LOCK,      // SDL_SCANCODE_CAPSLOCK
    KEY_F1,             // SDL_SCANCODE_F1
    KEY_F2,             // SDL_SCANCODE_F2
    KEY_F3,             // SDL_SCANCODE_F3
    KEY_F4,             // SDL_SCANCODE_F4
    KEY_F5,             // SDL_SCANCODE_F5
    KEY_F6,             // SDL_SCANCODE_F6
    KEY_F7,             // SDL_SCANCODE_F7
    KEY_F8,             // SDL_SCANCODE_F8
    KEY_F9,             // SDL_SCANCODE_F9
    KEY_F10,            // SDL_SCANCODE_F10
    KEY_F11,            // SDL_SCANCODE_F11
    KEY_F12,            // SDL_SCANCODE_F12
    KEY_PRINT_SCREEN,   // SDL_SCANCODE_PRINTSCREEN
    KEY_SCROLL_LOCK,    // SDL_SCANCODE_SCROLLLOCK
    KEY_PAUSE,          // SDL_SCANCODE_PAUSE
    KEY_INSERT,         // SDL_SCANCODE_INSERT
    KEY_HOME,           // SDL_SCANCODE_HOME
    KEY_PAGE_UP,        // SDL_SCANCODE_PAGEUP
    KEY_DELETE,         // SDL_SCANCODE_DELETE
    KEY_END,            // SDL_SCANCODE_END
    KEY_PAGE_DOWN,      // SDL_SCANCODE_PAGEDOWN
    KEY_RIGHT,          // SDL_SCANCODE_RIGHT
    KEY_LEFT,           // SDL_SCANCODE_LEFT
    KEY_DOWN,           // SDL_SCANCODE_DOWN
    KEY_UP,             // SDL_SCANCODE_UP
    KEY_NUM_LOCK,       // SDL_SCANCODE_NUMLOCKCLEAR
    KEY_KP_DIVIDE,      // SDL_SCANCODE_KP_DIVIDE
    KEY_KP_MULTIPLY,    // SDL_SCANCODE_KP_MULTIPLY
    KEY_KP_SUBTRACT,    // SDL_SCANCODE_KP_MINUS
    KEY_KP_ADD,         // SDL_SCANCODE_KP_PLUS
    KEY_KP_ENTER,       // SDL_SCANCODE_KP_ENTER
    KEY_KP_1,           // SDL_SCANCODE_KP_1
    KEY_KP_2,           // SDL_SCANCODE_KP_2
    KEY_KP_3,           // SDL_SCANCODE_KP_3
    KEY_KP_4,           // SDL_SCANCODE_KP_4
    KEY_KP_5,           // SDL_SCANCODE_KP_5
    KEY_KP_6,           // SDL_SCANCODE_KP_6
    KEY_KP_7,           // SDL_SCANCODE_KP_7
    KEY_KP_8,           // SDL_SCANCODE_KP_8
    KEY_KP_9,           // SDL_SCANCODE_KP_9
    KEY_KP_0,           // SDL_SCANCODE_KP_0
    KEY_KP_DECIMAL,     // SDL_SCANCODE_KP_PERIOD
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0,
    KEY_LEFT_CONTROL,   //SDL_SCANCODE_LCTRL
    KEY_LEFT_SHIFT,     //SDL_SCANCODE_LSHIFT
    KEY_LEFT_ALT,       //SDL_SCANCODE_LALT
    KEY_LEFT_SUPER,     //SDL_SCANCODE_LGUI
    KEY_RIGHT_CONTROL,  //SDL_SCANCODE_RCTRL
    KEY_RIGHT_SHIFT,    //SDL_SCANCODE_RSHIFT
    KEY_RIGHT_ALT,      //SDL_SCANCODE_RALT
    KEY_RIGHT_SUPER     //SDL_SCANCODE_RGUI
};

void Keyboard::setKeyState(int scancode, bool state)
{
    int key = ScancodeToKey[scancode];
    if ((key > 0) && (key < MAX_KEYS))
    {
        currentKeyState[key] = state;
       //   Log(0,"[KEYBOARD] Key %d %s %d",key,state?"down":"up",scancode);
    }

}