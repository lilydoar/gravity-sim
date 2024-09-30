#ifndef UI_HANDLER_H
#define UI_HANDLER_H

#include "gravity_interactor.h"
#include "arena_allocator.h"
#include "raylib.h"

typedef struct {
    bool is_selecting;
    Vector2 start_pos;
    Vector2 current_pos;
    SelectionType current_selection_type;
} UIState;

void handle_input(UIState* state, SimulationActor actor, ArenaAllocator* frame_arena);
void draw_ui(UIState state);

#endif // UI_HANDLER_H
