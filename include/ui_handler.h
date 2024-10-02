#ifndef UI_HANDLER_H
#define UI_HANDLER_H

#include "arena_allocator.h"
#include "gravity_interactor.h"
#include "raylib.h"

typedef struct {
  bool is_selecting;
  Vector2 start_pos;
  Vector2 current_pos;
  SelectionType current_selection_type;
  int *selected_particles;
  int selected_count;
  int selected_capacity;
  Rectangle make_static_button;
} UIState;

void init_ui_state(UIState *state);
void deinit_ui_state(UIState *state);
void add_to_selection(UIState *state, int particle_id);
void remove_from_selection(UIState *state, int particle_id);
void toggle_particle_selection(UIState *state, int particle_id);
void clear_selection(UIState *state);
bool is_particle_selected(UIState *state, int particle_id);
void handle_input(UIState *state, SimulationActor actor,
                  ArenaAllocator *frame_arena);
void draw_ui(UIState state, SimulationActor actor);

#endif // UI_HANDLER_H
