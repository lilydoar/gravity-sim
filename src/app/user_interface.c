#include "user_interface.h"

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define SELECT_BOX_COLOR                                                       \
  (Color) { 255, 0, 0, 255 }

// Forward declarations
Rectangle sort_rect(Vector2 p1, Vector2 p2);

// Draw the UI
// Update the UI state based on mouse input
bool draw_ui(UIState *state) {
  int tool_button_size = 40;
  int tool_spacing = 10;
  int tool_offset = 10;
  int tool_option_button_size = 30;

  bool button_pressed = false;

  // Draw the select tool
  Rectangle select_tool_rect = {tool_offset, tool_offset, tool_button_size,
                                tool_button_size};
  if (GuiButton(select_tool_rect, "Select")) {
    state->current_tool = UI_TOOL_SELECT;
    button_pressed = true;
  }

  // Draw the move tool
  Rectangle move_tool_rect = {select_tool_rect.x + tool_button_size +
                                  tool_spacing,
                              tool_offset, tool_button_size, tool_button_size};
  if (GuiButton(move_tool_rect, "Move")) {
    state->current_tool = UI_TOOL_MOVE;
    button_pressed = true;
  }

  // Draw the spawn tool
  Rectangle spawn_tool_rect = {move_tool_rect.x + tool_button_size +
                                   tool_spacing,
                               tool_offset, tool_button_size, tool_button_size};
  if (GuiButton(spawn_tool_rect, "Spawn")) {
    state->current_tool = UI_TOOL_SPAWN;
    button_pressed = true;
  }

  // Draw tool options
  if (state->current_tool == UI_TOOL_SELECT) {
    // Draw the select mode options
    Rectangle select_mode_rect = {
        select_tool_rect.x,
        select_tool_rect.y + tool_button_size + tool_spacing,
        tool_option_button_size, tool_option_button_size};
    if (GuiButton(select_mode_rect, "Box")) {
      state->select_mode = SELECT_MODE_BOX;
      button_pressed = true;
    }
  } else if (state->current_tool == UI_TOOL_MOVE) {
    // Draw the move tool options
    Rectangle move_mode_rect = {
        move_tool_rect.x, move_tool_rect.y + tool_button_size + tool_spacing,
        tool_option_button_size, tool_option_button_size};
    if (GuiButton(move_mode_rect, "Position")) {
      state->move_mode = MOVE_MODE_POSITION;
      button_pressed = true;
    }
  }

  return button_pressed;
}

void draw_tool(UIState *state, UserInput *input) {
  if (state->current_tool == UI_TOOL_SELECT) {
    if (state->select_mode == SELECT_MODE_BOX) {
      if (input->mouse_left_pressed) {
        Rectangle select_box_rect =
            sort_rect(input->mouse_start, input->mouse_current);
        DrawRectangleLinesEx(select_box_rect, 1, SELECT_BOX_COLOR);
      }
    }
  } else if (state->current_tool == UI_TOOL_MOVE) {
  } else if (state->current_tool == UI_TOOL_SPAWN) {
    if (input->mouse_left_pressed) {
      float radius = Vector2Distance(input->mouse_start, input->mouse_current);
      DrawCircleLinesV(input->mouse_start, radius, WHITE);
    }
  }
}

// Sort the start and end of the selection box and return the rectangle
Rectangle sort_rect(Vector2 p1, Vector2 p2) {
  float x1 = fminf(p1.x, p2.x);
  float x2 = fmaxf(p1.x, p2.x);
  float y1 = fminf(p1.y, p2.y);
  float y2 = fmaxf(p1.y, p2.y);
  return (Rectangle){x1, y1, x2 - x1, y2 - y1};
}