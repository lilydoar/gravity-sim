#include "user_input.h"

#include "raylib.h"

void collect_input(UserInput *input) {
  input->mouse_left_released = false;
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    input->mouse_left_pressed = true;
    input->mouse_start = GetMousePosition();
  } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    input->mouse_left_pressed = false;
    input->mouse_left_released = true;
  }
  input->mouse_current = GetMousePosition();
}