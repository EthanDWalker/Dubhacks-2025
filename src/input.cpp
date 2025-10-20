#include "input.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <queue>

using std::queue;

__uint128_t InputContext::_pressed_input = 0;
__uint128_t InputContext::_held_input = 0;
__uint128_t InputContext::_released_input = 0;
glm::vec2 InputContext::window_size = glm::vec2(1.0f);

glm::vec2 InputContext::mouse_position = glm::vec2(0.0f);
glm::vec2 InputContext::delta_mouse_position = glm::vec2(0.0f);
queue<std::filesystem::path> InputContext::dropped_file_queue = {};

void InputContext::_DropCallback(GLFWwindow *window, int32_t path_count,
                                 const char *paths[]) {
  for (int32_t i = 0; i < path_count; i++) {
    dropped_file_queue.push(paths[i]);
  }
}

void InputContext::InitCallbacks(GLFWwindow *window) {
  glfwSetDropCallback(window, _DropCallback);
};

void InputContext::Update(GLFWwindow *window) {
  glfwPollEvents();

  glm::ivec2 window_size_int;
  glfwGetWindowSize(window, &window_size_int.x, &window_size_int.y);
  glm::dvec2 mouse_position_double;
  glfwGetCursorPos(window, &mouse_position_double.x, &mouse_position_double.y);

  window_size = static_cast<glm::vec2>(window_size_int);

  glm::vec2 new_mouse_position =
      (static_cast<glm::vec2>(mouse_position_double) + glm::vec2(0.5f)) /
      window_size;
  new_mouse_position = new_mouse_position * 2.0f - 1.0f;

  delta_mouse_position = mouse_position - new_mouse_position;
  mouse_position = new_mouse_position;

  _pressed_input = 0;
  _released_input = 0;
  for (uint8_t i = 0; i < Input::KEYBOARD_COUNT; i++) {
    __uint128_t input_down = glfwGetKey(window, KEY_TO_GLFW_KEY[i]) &&
                             !ImGui::GetIO().WantCaptureKeyboard;

    _pressed_input |=
        __uint128_t(!GetInputHeld((Input::Input)i) && bool(input_down)) << i;

    _released_input |=
        __uint128_t(GetInputHeld((Input::Input)i) && !bool(input_down)) << i;
  }

  for (uint8_t i = Input::KEYBOARD_COUNT; i < Input::COUNT; i++) {
    __uint128_t input_down =
        glfwGetMouseButton(window,
                           MOUSE_TO_GLFW_MOUSE[i - Input::KEYBOARD_COUNT]) &&
        !ImGui::GetIO().WantCaptureMouse;

    _pressed_input |=
        __uint128_t(!GetInputHeld((Input::Input)i) && bool(input_down)) << i;

    _released_input |=
        __uint128_t(GetInputHeld((Input::Input)i) && !bool(input_down)) << i;
  }

  _held_input = 0;
  for (uint8_t i = 0; i < Input::KEYBOARD_COUNT; i++) {
    __uint128_t input_down = glfwGetKey(window, KEY_TO_GLFW_KEY[i]) &&
                             !ImGui::GetIO().WantCaptureKeyboard;

    _held_input |= input_down << i;
  }

  for (uint8_t i = Input::KEYBOARD_COUNT; i < Input::COUNT; i++) {
    __uint128_t input_down =
        glfwGetMouseButton(window,
                           MOUSE_TO_GLFW_MOUSE[i - Input::KEYBOARD_COUNT]) &&
        !ImGui::GetIO().WantCaptureMouse;

    _held_input |= input_down << i;
  }
}
