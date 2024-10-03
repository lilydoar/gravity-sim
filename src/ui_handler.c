#include "ui_handler.h"
#include "gravity_interactor.h"
#include "logging.h"
#include "raymath.h"
#include "raygui.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define INITIAL_SELECTION_CAPACITY 100

void init_ui_state(UIState *state) {
  state->is_selecting = false;
  state->selected_particles = malloc(INITIAL_SELECTION_CAPACITY * sizeof(int));
  state->selected_count = 0;
  state->selected_capacity = INITIAL_SELECTION_CAPACITY;
  state->make_static_button = (Rectangle){10, 10, 150, 30};
}

void deinit_ui_state(UIState *state) { free(state->selected_particles); }

void add_to_selection(UIState *state, int particle_id) {
  // Check if the particle is already selected
  for (int i = 0; i < state->selected_count; i++) {
    if (state->selected_particles[i] == particle_id) {
      return; // Particle is already selected, do nothing
    }
  }

  // Add particle to selection
  if (state->selected_count == state->selected_capacity) {
    state->selected_capacity *= 2;
    state->selected_particles = realloc(state->selected_particles,
                                        state->selected_capacity * sizeof(int));
  }
  state->selected_particles[state->selected_count++] = particle_id;
}

void remove_from_selection(UIState *state, int particle_id) {
  for (int i = 0; i < state->selected_count; i++) {
    if (state->selected_particles[i] == particle_id) {
      // Remove particle from selection
      for (int j = i; j < state->selected_count - 1; j++) {
        state->selected_particles[j] = state->selected_particles[j + 1];
      }
      state->selected_count--;
      return;
    }
  }
}

void toggle_particle_selection(UIState *state, int particle_id) {
  for (int i = 0; i < state->selected_count; i++) {
    if (state->selected_particles[i] == particle_id) {
      remove_from_selection(state, particle_id);
      return;
    }
  }
  add_to_selection(state, particle_id);
}

void clear_selection(UIState *state) { state->selected_count = 0; }

bool is_particle_selected(UIState *state, int particle_id) {
  for (int i = 0; i < state->selected_count; i++) {
    if (state->selected_particles[i] == particle_id) {
      return true;
    }
  }
  return false;
}

void handle_input(UIState *state, SimulationActor actor,
                  ArenaAllocator *frame_arena) {
  extern Camera2D camera;
  Vector2 mouse_pos = GetMousePosition();
  Vector2 world_mouse_pos = GetScreenToWorld2D(mouse_pos, camera);

  TRACE_LOG("Mouse position: (%f, %f)", mouse_pos.x, mouse_pos.y);
  TRACE_LOG("World mouse position: (%f, %f)", world_mouse_pos.x,
            world_mouse_pos.y);
  TRACE_LOG("Camera position: (%f, %f), zoom: %f", camera.target.x,
            camera.target.y, camera.zoom);

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (CheckCollisionPointRec(mouse_pos, state->make_static_button)) {
      DEBUG_LOG("Make static button clicked");
      // Handle button click
      ParticleSelection selection = {.particle_ids = state->selected_particles,
                                     .count = state->selected_count};
      Action action = create_action(frame_arena, ACTION_MAKE_STATIC, selection);
      enqueue_action(&actor->queue, action);
      DEBUG_LOG("Created action to make %d particles static",
                action.selection.count);
    } else {
      state->is_selecting = true;
      state->start_pos = world_mouse_pos;
      state->current_selection_type = SELECTION_RECTANGLE;
      DEBUG_LOG("Started rectangle selection at (%f, %f)", state->start_pos.x,
                state->start_pos.y);
    }
  } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    state->is_selecting = true;
    state->start_pos = world_mouse_pos;
    state->current_selection_type = SELECTION_CIRCLE;
    DEBUG_LOG("Started circle selection at (%f, %f)", state->start_pos.x,
              state->start_pos.y);
  }

  if (state->is_selecting) {
    state->current_pos = world_mouse_pos;
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) ||
        IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
      state->is_selecting = false;

      ParticleSelection selection;
      selection.type = state->current_selection_type;

      if (selection.type == SELECTION_RECTANGLE) {
        selection.shape.rectangle.top_left =
            (vec2s){{fmin(state->start_pos.x, state->current_pos.x),
                     fmin(state->start_pos.y, state->current_pos.y)}};
        selection.shape.rectangle.bottom_right =
            (vec2s){{fmax(state->start_pos.x, state->current_pos.x),
                     fmax(state->start_pos.y, state->current_pos.y)}};
        DEBUG_LOG("Ended rectangle selection: (%f, %f) to (%f, %f)",
                  selection.shape.rectangle.top_left.x,
                  selection.shape.rectangle.top_left.y,
                  selection.shape.rectangle.bottom_right.x,
                  selection.shape.rectangle.bottom_right.y);
      } else {
        selection.shape.circle.center =
            (vec2s){{state->start_pos.x, state->start_pos.y}};
        selection.shape.circle.radius =
            Vector2Distance(state->start_pos, state->current_pos);
        DEBUG_LOG("Ended circle selection: center (%f, %f), radius %f",
                  selection.shape.circle.center.x,
                  selection.shape.circle.center.y,
                  selection.shape.circle.radius);
      }

      int max_particles = 1000;
      int *particle_ids = malloc(max_particles * sizeof(int));
      if (particle_ids == NULL) {
        DEBUG_LOG("Failed to allocate memory for particle_ids");
        return;
      }
      int count;

      if (selection.type == SELECTION_RECTANGLE) {
        DEBUG_LOG("Calling get_particles_in_rectangle with top_left (%.2f, "
                  "%.2f), bottom_right (%.2f, %.2f)",
                  selection.shape.rectangle.top_left.x,
                  selection.shape.rectangle.top_left.y,
                  selection.shape.rectangle.bottom_right.x,
                  selection.shape.rectangle.bottom_right.y);
        count = get_particles_in_rectangle(
            actor->sim, selection.shape.rectangle.top_left,
            selection.shape.rectangle.bottom_right, particle_ids,
            max_particles);
        DEBUG_LOG("get_particles_in_rectangle returned %d", count);
      } else {
        DEBUG_LOG("Calling get_particles_in_circle with center (%.2f, %.2f), "
                  "radius %.2f",
                  selection.shape.circle.center.x,
                  selection.shape.circle.center.y,
                  selection.shape.circle.radius);
        count = get_particles_in_circle(
            actor->sim, selection.shape.circle.center,
            selection.shape.circle.radius, particle_ids, max_particles);
      }
      DEBUG_LOG("Found %d particles in selection", count);

      if (count < 0) {
        ERROR_LOG("Error in particle selection, count is negative");
      } else if (count == 0) {
        DEBUG_LOG("No particles found in the selection area");
        clear_selection(state);
      } else {
        DEBUG_LOG("Particles found in selection:");
        for (int i = 0; i < count && i < 10; i++) {
          Particle p = get_particle_state(actor->sim, particle_ids[i]);
          DEBUG_LOG("Particle %d: position (%.2f, %.2f)", particle_ids[i],
                    p.position.x, p.position.y);
        }
        if (count > 10) {
          DEBUG_LOG("... and %d more", count - 10);
        }
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
          DEBUG_LOG("Adding to selection");
          // Add to selection
          for (int i = 0; i < count; i++) {
            add_to_selection(state, particle_ids[i]);
          }
        } else if (IsKeyDown(KEY_LEFT_CONTROL)) {
          DEBUG_LOG("Toggling selection (CTRL pressed)");
          // Toggle selection
          for (int i = 0; i < count; i++) {
            if (is_particle_selected(state, particle_ids[i])) {
              remove_from_selection(state, particle_ids[i]);
              DEBUG_LOG("Removed particle %d from selection", particle_ids[i]);
            } else {
              add_to_selection(state, particle_ids[i]);
              DEBUG_LOG("Added particle %d to selection", particle_ids[i]);
            }
          }
        } else {
          DEBUG_LOG("New selection");
          // New selection
          clear_selection(state);
          for (int i = 0; i < count; i++) {
            add_to_selection(state, particle_ids[i]);
          }
        }
      }

      free(particle_ids);
      DEBUG_LOG("Selected %d particles", state->selected_count);
    }
  }
}

void draw_ui(UIState state, SimulationActor actor, ArenaAllocator *frame_arena) {
  extern Camera2D camera;
  TRACE_LOG("Drawing UI");
  if (state.is_selecting) {
    TRACE_LOG("Drawing selection");
    if (state.current_selection_type == SELECTION_RECTANGLE) {
      Vector2 start_screen = GetWorldToScreen2D(
          (Vector2){state.start_pos.x, state.start_pos.y}, camera);
      Vector2 current_screen = GetWorldToScreen2D(
          (Vector2){state.current_pos.x, state.current_pos.y}, camera);
      float x = fmin(start_screen.x, current_screen.x);
      float y = fmin(start_screen.y, current_screen.y);
      float width = fabs(current_screen.x - start_screen.x);
      float height = fabs(current_screen.y - start_screen.y);
      DrawRectangleLines((int)x, (int)y, (int)width, (int)height, RED);
      TRACE_LOG("Drawing rectangle: (%f, %f, %f, %f)", x, y, width, height);
    } else {
      Vector2 center_screen = GetWorldToScreen2D(
          (Vector2){state.start_pos.x, state.start_pos.y}, camera);
      float radius =
          Vector2Distance(state.start_pos, state.current_pos) * camera.zoom;
      DrawCircleLines((int)center_screen.x, (int)center_screen.y, radius, RED);
      TRACE_LOG("Drawing circle: center (%f, %f), radius %f", center_screen.x,
                center_screen.y, radius);
    }
  }

  // Draw selected particles
  TRACE_LOG("Drawing %d selected particles", state.selected_count);
  for (int i = 0; i < state.selected_count; i++) {
    Particle p = get_particle_state(actor->sim, state.selected_particles[i]);
    Vector2 screen_pos =
        GetWorldToScreen2D((Vector2){p.position.x, p.position.y}, camera);
    DrawCircleLines((int)screen_pos.x, (int)screen_pos.y,
                    (p.size + 2) * camera.zoom, YELLOW);
  }

  // Draw "Make Static" button
  if (GuiButton(state.make_static_button, "Make Static")) {
    DEBUG_LOG("Make static button clicked");
    ParticleSelection selection = {.particle_ids = state.selected_particles,
                                   .count = state.selected_count};
    Action action = create_action(frame_arena, ACTION_MAKE_STATIC, selection);
    enqueue_action(&actor->queue, action);
    DEBUG_LOG("Created action to make %d particles static",
              action.selection.count);
  }
  TRACE_LOG("Drew Make Static button using raygui");
}
