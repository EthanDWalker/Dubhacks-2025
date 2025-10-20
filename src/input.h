#pragma once

#include <GLFW/glfw3.h>
#include <filesystem>
#include <glm/vec2.hpp>
#include <queue>

namespace Input {
enum Input : uint8_t {
  SPACE,
  APOSTROPHE, /* ' */
  COMMA,      /* , */
  MINUS,      /* - */
  PERIOD,     /* . */
  SLASH,      /* / */
  ZERO,
  ONE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  SEMICOLON, /* ; */
  EQUAL,     /* = */
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  LEFT_BRACKET,  /* [ */
  BACKSLASH,     /* \ */
  RIGHT_BRACKET, /* ] */
  GRAVE_ACCENT,  /* ` */
  WORLD_1,       /* non-US #1 */
  WORLD_2,       /* non-US #2 */
  ESCAPE,
  ENTER,
  TAB,
  BACKSPACE,
  INSERT,
  DELETE,
  RIGHT,
  LEFT,
  DOWN,
  UP,
  PAGE_UP,
  PAGE_DOWN,
  HOME,
  END,
  CAPS_LOCK,
  SCROLL_LOCK,
  NUM_LOCK,
  PRINT_SCREEN,
  PAUSE,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  F13,
  F14,
  F15,
  F16,
  F17,
  F18,
  F19,
  F20,
  F21,
  F22,
  F23,
  F24,
  F25,
  NUM_PAD_0,
  NUM_PAD_1,
  NUM_PAD_2,
  NUM_PAD_3,
  NUM_PAD_4,
  NUM_PAD_5,
  NUM_PAD_6,
  NUM_PAD_7,
  NUM_PAD_8,
  NUM_PAD_9,
  NUM_PAD_DECIMAL,
  NUM_PAD_DIVIDE,
  NUM_PAD_MULTIPLY,
  NUM_PAD_SUBTRACT,
  NUM_PAD_ADD,
  NUM_PAD_ENTER,
  NUM_PAD_EQUAL,
  LEFT_SHIFT,
  LEFT_CONTROL,
  LEFT_ALT,
  LEFT_SUPER,
  RIGHT_SHIFT,
  RIGHT_CONTROL,
  RIGHT_ALT,
  RIGHT_SUPER,
  MENU,
  MOUSE_LEFT,
  MOUSE_RIGHT,
  MOUSE_MIDDLE,
  KEYBOARD_COUNT = MENU + 1,
  COUNT = MOUSE_MIDDLE + 1,
};
}

const uint16_t KEY_TO_GLFW_KEY[static_cast<size_t>(Input::KEYBOARD_COUNT)] = {
    32,  39,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,
    57,  59,  61,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,
    77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,
    92,  93,  96,  161, 162, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265,
    266, 267, 268, 269, 280, 281, 282, 283, 284, 290, 291, 292, 293, 294, 295,
    296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310,
    311, 312, 313, 314, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330,
    331, 332, 333, 334, 335, 336, 340, 341, 342, 343, 344, 345, 346, 347, 348,
};

const uint16_t MOUSE_TO_GLFW_MOUSE[static_cast<size_t>(Input::COUNT) -
                                   static_cast<size_t>(Input::KEYBOARD_COUNT)] =
    {
        0,
        1,
        2,
};

struct InputContext {
  static __uint128_t _pressed_input;
  static __uint128_t _held_input;
  static __uint128_t _released_input;

  static glm::vec2 window_size;
  static glm::vec2 mouse_position;
  static glm::vec2 delta_mouse_position;

  static std::queue<std::filesystem::path> dropped_file_queue;

  static void _DropCallback(GLFWwindow *window, int32_t path_count,
                            const char *paths[]);

  static void InitCallbacks(GLFWwindow *window);

  static void Update(GLFWwindow *window);

  static inline const bool GetInputHeld(Input::Input input) {
    return (_held_input >> static_cast<__uint128_t>(input)) & 0b1;
  };

  static inline const bool GetInputPressed(Input::Input input) {
    return (_pressed_input >> static_cast<__uint128_t>(input)) & 0b1;
  };

  static inline const bool GetInputRelease(Input::Input input) {
    return (_released_input >> static_cast<__uint128_t>(input)) & 0b1;
  };
};
