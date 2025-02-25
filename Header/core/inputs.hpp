#pragma once

#include "GLFW/glfw3.h"

namespace dg
{

  enum class Key : uint16_t
  {
    unknown = 0,

    /* Printable keys */
    space = GLFW_KEY_SPACE,
    apostrophe = GLFW_KEY_APOSTROPHE,
    comma = GLFW_KEY_COMMA, /* , */
    minus = GLFW_KEY_MINUS, /* - */
    period = GLFW_KEY_PERIOD, /* , */
    slash = GLFW_KEY_SLASH, /* / */

    zero = GLFW_KEY_0,
    one = GLFW_KEY_1,
    two = GLFW_KEY_2,
    three = GLFW_KEY_3,
    four = GLFW_KEY_4,
    five = GLFW_KEY_5,
    six = GLFW_KEY_6,
    seven = GLFW_KEY_7,
    height = GLFW_KEY_8,
    nine = GLFW_KEY_9,

    semicolon = GLFW_KEY_SEMICOLON, /* ; */
    equal = GLFW_KEY_EQUAL, /* =*/

    a = GLFW_KEY_A,
    b = GLFW_KEY_B,
    c = GLFW_KEY_C,
    d = GLFW_KEY_D,
    e = GLFW_KEY_E,
    f = GLFW_KEY_F,
    g = GLFW_KEY_G,
    h = GLFW_KEY_H,
    i = GLFW_KEY_I,
    j = GLFW_KEY_J,
    k = GLFW_KEY_K,
    l = GLFW_KEY_L,
    m = GLFW_KEY_M,
    n = GLFW_KEY_N,
    o = GLFW_KEY_O,
    p = GLFW_KEY_P,
    q = GLFW_KEY_Q,
    r = GLFW_KEY_R,
    s = GLFW_KEY_S,
    t = GLFW_KEY_T,
    u = GLFW_KEY_U,
    v = GLFW_KEY_V,
    w = GLFW_KEY_W,
    x = GLFW_KEY_X,
    y = GLFW_KEY_Y,
    z = GLFW_KEY_Z,

    leftBracket = GLFW_KEY_LEFT_BRACKET, /* [ */
    backslash = GLFW_KEY_BACKSLASH, /* \ */
    rightBracket = GLFW_KEY_RIGHT_BRACKET, /* ] */
    graveAccent = GLFW_KEY_GRAVE_ACCENT, /* ` */

    world1 = GLFW_KEY_WORLD_1, /* non-US #1 */
    world2 = GLFW_KEY_WORLD_2, /* non-US #2 */

    escape = GLFW_KEY_ESCAPE,
    enter = GLFW_KEY_ENTER,
    tab = GLFW_KEY_TAB,
    backspace = GLFW_KEY_BACKSPACE,
    insert = GLFW_KEY_INSERT,
    del = GLFW_KEY_DELETE,
    right = GLFW_KEY_RIGHT,
    left = GLFW_KEY_LEFT,
    down = GLFW_KEY_DOWN,
    up = GLFW_KEY_UP,
    pageUp = GLFW_KEY_PAGE_UP,
    pageDown = GLFW_KEY_PAGE_DOWN,
    home = GLFW_KEY_HOME,
    end = GLFW_KEY_END,
    lock = GLFW_KEY_CAPS_LOCK,
    scrollLock = GLFW_KEY_SCROLL_LOCK,
    numLock = GLFW_KEY_NUM_LOCK,
    printScreen = GLFW_KEY_PRINT_SCREEN,
    pause = GLFW_KEY_PAUSE,

    f1 = GLFW_KEY_F1,
    f2 = GLFW_KEY_F2,
    f3 = GLFW_KEY_F3,
    f4 = GLFW_KEY_F4,
    f5 = GLFW_KEY_F5,
    f6 = GLFW_KEY_F6,
    f7 = GLFW_KEY_F7,
    f8 = GLFW_KEY_F8,
    f9 = GLFW_KEY_F9,
    f10 = GLFW_KEY_F10,
    f11 = GLFW_KEY_F11,
    f12 = GLFW_KEY_F12,
    f13 = GLFW_KEY_F13,
    f14 = GLFW_KEY_F14,
    f15 = GLFW_KEY_F15,
    f16 = GLFW_KEY_F16,
    f17 = GLFW_KEY_F17,
    f18 = GLFW_KEY_F18,
    f19 = GLFW_KEY_F19,
    f20 = GLFW_KEY_F20,
    f21 = GLFW_KEY_F21,
    f22 = GLFW_KEY_F22,
    f23 = GLFW_KEY_F23,
    f24 = GLFW_KEY_F24,
    f25 = GLFW_KEY_F25,

    kp0 = GLFW_KEY_KP_0,
    kp1 = GLFW_KEY_KP_1,
    kp2 = GLFW_KEY_KP_2,
    kp3 = GLFW_KEY_KP_3,
    kp4 = GLFW_KEY_KP_4,
    kp5 = GLFW_KEY_KP_5,
    kp6 = GLFW_KEY_KP_6,
    kp7 = GLFW_KEY_KP_7,
    kp8 = GLFW_KEY_KP_8,
    kp9 = GLFW_KEY_KP_9,
    kpDecimal = GLFW_KEY_KP_DECIMAL,
    kpDivide = GLFW_KEY_KP_DIVIDE,
    kpMultiply = GLFW_KEY_KP_MULTIPLY,
    kpSubtract = GLFW_KEY_KP_SUBTRACT,
    kpAdd = GLFW_KEY_KP_ADD,
    kpEnter = GLFW_KEY_KP_ENTER,
    kpEqual = GLFW_KEY_KP_EQUAL,
    leftShift = GLFW_KEY_LEFT_SHIFT,
    leftControl = GLFW_KEY_LEFT_CONTROL,
    leftAlt = GLFW_KEY_LEFT_ALT,
    leftSuper = GLFW_KEY_LEFT_SUPER,
    rightShift = GLFW_KEY_RIGHT_SHIFT,
    rightControl = GLFW_KEY_RIGHT_CONTROL,
    rightAlt = GLFW_KEY_RIGHT_ALT,
    rightSuper = GLFW_KEY_RIGHT_SUPER,
    menu = GLFW_KEY_MENU,
  };

  enum class KeyboardMods : uint8_t
  {
    shift = GLFW_MOD_SHIFT,
    control = GLFW_MOD_CONTROL,
    alt = GLFW_MOD_ALT,
    super = GLFW_MOD_SUPER,
    capsLock = GLFW_MOD_CAPS_LOCK,
    numLock = GLFW_MOD_NUM_LOCK,
  };

  enum class MouseButton : uint8_t
  {
    one = GLFW_MOUSE_BUTTON_1,
    two = GLFW_MOUSE_BUTTON_2,
    three = GLFW_MOUSE_BUTTON_3,
    four = GLFW_MOUSE_BUTTON_4,
    five = GLFW_MOUSE_BUTTON_5,
    six = GLFW_MOUSE_BUTTON_6,
    seven = GLFW_MOUSE_BUTTON_7,
    height = GLFW_MOUSE_BUTTON_8,
    left = GLFW_MOUSE_BUTTON_LEFT,
    right = GLFW_MOUSE_BUTTON_RIGHT,
    middle = GLFW_MOUSE_BUTTON_MIDDLE,
  };

  enum class Joystick : uint8_t
  {
    one = GLFW_JOYSTICK_1,
    two = GLFW_JOYSTICK_2,
    three = GLFW_JOYSTICK_3,
    four = GLFW_JOYSTICK_4,
    five = GLFW_JOYSTICK_5,
    six = GLFW_JOYSTICK_6,
    seven = GLFW_JOYSTICK_7,
    height = GLFW_JOYSTICK_8,
    nine = GLFW_JOYSTICK_9,
    ten = GLFW_JOYSTICK_10,
    eleven = GLFW_JOYSTICK_11,
    twelve = GLFW_JOYSTICK_12,
    thirdteen = GLFW_JOYSTICK_13,
    fourteen = GLFW_JOYSTICK_14,
    fiveteen = GLFW_JOYSTICK_15,
    sixteen = GLFW_JOYSTICK_16,
  };

  enum class GamePadButton : uint8_t
  {
    a = GLFW_GAMEPAD_BUTTON_A,
    b = GLFW_GAMEPAD_BUTTON_B,
    x = GLFW_GAMEPAD_BUTTON_X,
    y = GLFW_GAMEPAD_BUTTON_Y,

    leftBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
    rightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
    back = GLFW_GAMEPAD_BUTTON_BACK,
    start = GLFW_GAMEPAD_BUTTON_START,
    guide = GLFW_GAMEPAD_BUTTON_GUIDE,
    leftThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
    rightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
    dpadUp = GLFW_GAMEPAD_BUTTON_DPAD_UP,
    dpadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
    dpadDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
    dpadLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,

    cross = GLFW_GAMEPAD_BUTTON_CROSS,
    circle = GLFW_GAMEPAD_BUTTON_CIRCLE,
    square = GLFW_GAMEPAD_BUTTON_SQUARE,
    triangle = GLFW_GAMEPAD_BUTTON_TRIANGLE,
  };

  enum class GamePadAxis : uint8_t
  {
    leftX = GLFW_GAMEPAD_AXIS_LEFT_X,
    leftY = GLFW_GAMEPAD_AXIS_LEFT_Y,
    rightX = GLFW_GAMEPAD_AXIS_RIGHT_X,
    rightY = GLFW_GAMEPAD_AXIS_RIGHT_Y,
    leftTrigger = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
    rightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
  };

} // dg
