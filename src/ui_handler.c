#include "ui_handler.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>

#define DEBUG_LOG(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__)

void handle_input(UIState *state, SimulationActor actor,
                  ArenaAllocator *frame_arena) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    state->is_selecting = true;
    state->start_pos = GetMousePosition();
    state->current_selection_type = SELECTION_RECTANGLE;
  } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    state->is_selecting = true;
    state->start_pos = GetMousePosition();
    state->current_selection_type = SELECTION_CIRCLE;
  }

  if (state->is_selecting) {
    state->current_pos = GetMousePosition();
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) ||
        IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
      state->is_selecting = false;

      ParticleSelection selection;
      selection.type = state->current_selection_type;

      if (selection.type == SELECTION_RECTANGLE) {
        extern Camera2D camera; // Ensure the camera is declared as external
        Vector2 world_start_pos = GetScreenToWorld2D(state->start_pos, camera);
        Vector2 world_current_pos =
            GetScreenToWorld2D(state->current_pos, camera);
        selection.shape.rectangle.top_left =
            (vec2s){{(double)fmin(world_start_pos.x, world_current_pos.x),
                     (double)fmin(world_start_pos.y, world_current_pos.y)}};
        selection.shape.rectangle.bottom_right =
            (vec2s){{(double)fmax(world_start_pos.x, world_current_pos.x),
                     (double)fmax(world_start_pos.y, world_current_pos.y)}};
        DEBUG_LOG("Starting action: Creating rectangle selection from (%f, %f) "
                  "to (%f, %f)",
                  selection.shape.rectangle.top_left.x,
                  selection.shape.rectangle.top_left.y,
                  selection.shape.rectangle.bottom_right.x,
                  selection.shape.rectangle.bottom_right.y);
        Action action =
            create_action(frame_arena, ACTION_MAKE_STATIC, selection);
        enqueue_action(&actor->queue, action);
        DEBUG_LOG("Completed action: Created action of type %d",
                  ACTION_MAKE_STATIC);
      } else {
        selection.shape.circle.center =
            (vec2s){{(double)state->start_pos.x, (double)state->start_pos.y}};
        selection.shape.circle.radius =
            Vector2Distance(state->start_pos, state->current_pos);
        // For now, we're not creating an action for circular selections
      }
    }
  }
}

void draw_ui(UIState state) {
  if (state.is_selecting) {
    if (state.current_selection_type == SELECTION_RECTANGLE) {
      DrawRectangleLines(fmin(state.start_pos.x, state.current_pos.x),
                         fmin(state.start_pos.y, state.current_pos.y),
                         fabs(state.current_pos.x - state.start_pos.x),
                         fabs(state.current_pos.y - state.start_pos.y), RED);
    } else {
      float radius = Vector2Distance(state.start_pos, state.current_pos);
      DrawCircleLines(state.start_pos.x, state.start_pos.y, radius, RED);
    }
  }
}
