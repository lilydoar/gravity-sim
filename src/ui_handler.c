#include "ui_handler.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>

#define DEBUG_LOG(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__)

void handle_input(UIState* state, SimulationActor actor, ArenaAllocator* frame_arena) {
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
        DEBUG_LOG("Selection in progress: (%f, %f) to (%f, %f)", 
            state->start_pos.x, state->start_pos.y, 
            state->current_pos.x, state->current_pos.y);

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            state->is_selecting = false;

            ParticleSelection selection;
            selection.type = state->current_selection_type;

            if (selection.type == SELECTION_RECTANGLE) {
                selection.shape.rectangle.top_left = (Vector2D){(double)fmin(state->start_pos.x, state->current_pos.x), (double)fmin(state->start_pos.y, state->current_pos.y)};
                selection.shape.rectangle.bottom_right = (Vector2D){(double)fmax(state->start_pos.x, state->current_pos.x), (double)fmax(state->start_pos.y, state->current_pos.y)};
                Action action = create_action(frame_arena, ACTION_MAKE_STATIC, selection);
                enqueue_action(&actor->queue, action);
                DEBUG_LOG("Selection completed, creating action of type %d", ACTION_MAKE_STATIC);
            } else {
                selection.shape.circle.center = (Vector2D){(double)state->start_pos.x, (double)state->start_pos.y};
                selection.shape.circle.radius = Vector2Distance(state->start_pos, state->current_pos);
                // For now, we're not creating an action for circular selections
            }
        }
    }
}

void draw_ui(UIState state) {
    if (state.is_selecting) {
        if (state.current_selection_type == SELECTION_RECTANGLE) {
            DrawRectangleLines(
                fmin(state.start_pos.x, state.current_pos.x),
                fmin(state.start_pos.y, state.current_pos.y),
                fabs(state.current_pos.x - state.start_pos.x),
                fabs(state.current_pos.y - state.start_pos.y),
                RED
            );
        } else {
            float radius = Vector2Distance(state.start_pos, state.current_pos);
            DrawCircleLines(state.start_pos.x, state.start_pos.y, radius, RED);
        }
    }
}
