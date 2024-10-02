#include "ui_handler.h"
#include "raymath.h"
#include "gravity_interactor.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_LOG(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__)
#define INITIAL_SELECTION_CAPACITY 100

void init_ui_state(UIState* state) {
    state->is_selecting = false;
    state->selected_particles = malloc(INITIAL_SELECTION_CAPACITY * sizeof(int));
    state->selected_count = 0;
    state->selected_capacity = INITIAL_SELECTION_CAPACITY;
    state->make_static_button = (Rectangle){ 10, 10, 150, 30 };
}

void deinit_ui_state(UIState* state) {
    free(state->selected_particles);
}

void toggle_particle_selection(UIState* state, int particle_id) {
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
    
    // Add particle to selection
    if (state->selected_count == state->selected_capacity) {
        state->selected_capacity *= 2;
        state->selected_particles = realloc(state->selected_particles, state->selected_capacity * sizeof(int));
    }
    state->selected_particles[state->selected_count++] = particle_id;
}

void clear_selection(UIState* state) {
    state->selected_count = 0;
}

void handle_input(UIState *state, SimulationActor actor, ArenaAllocator *frame_arena) {
    extern Camera2D camera;
    Vector2 mouse_pos = GetMousePosition();
    Vector2 world_mouse_pos = GetScreenToWorld2D(mouse_pos, camera);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse_pos, state->make_static_button)) {
            // Handle button click
            ParticleSelection selection = {
                .type = SELECTION_RECTANGLE,
                .particle_ids = state->selected_particles,
                .count = state->selected_count
            };
            Action action = create_action(frame_arena, ACTION_MAKE_STATIC, selection);
            enqueue_action(&actor->queue, action);
            DEBUG_LOG("Created action to make %d particles static", state->selected_count);
        } else {
            state->is_selecting = true;
            state->start_pos = world_mouse_pos;
            state->current_selection_type = SELECTION_RECTANGLE;
        }
    } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        state->is_selecting = true;
        state->start_pos = world_mouse_pos;
        state->current_selection_type = SELECTION_CIRCLE;
    }

    if (state->is_selecting) {
        state->current_pos = world_mouse_pos;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            state->is_selecting = false;

            ParticleSelection selection;
            selection.type = state->current_selection_type;

            if (selection.type == SELECTION_RECTANGLE) {
                selection.shape.rectangle.top_left = (vec2s){{fmin(state->start_pos.x, state->current_pos.x), fmin(state->start_pos.y, state->current_pos.y)}};
                selection.shape.rectangle.bottom_right = (vec2s){{fmax(state->start_pos.x, state->current_pos.x), fmax(state->start_pos.y, state->current_pos.y)}};
            } else {
                selection.shape.circle.center = (vec2s){{state->start_pos.x, state->start_pos.y}};
                selection.shape.circle.radius = Vector2Distance(state->start_pos, state->current_pos);
            }

            int max_particles = 1000;
            int* particle_ids = malloc(max_particles * sizeof(int));
            int count;

            if (selection.type == SELECTION_RECTANGLE) {
                count = get_particles_in_rectangle(actor->sim, selection.shape.rectangle.top_left, selection.shape.rectangle.bottom_right, particle_ids, max_particles);
            } else {
                count = get_particles_in_circle(actor->sim, selection.shape.circle.center, selection.shape.circle.radius, particle_ids, max_particles);
            }

            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                // Add to selection
                for (int i = 0; i < count; i++) {
                    toggle_particle_selection(state, particle_ids[i]);
                }
            } else if (IsKeyDown(KEY_LEFT_CONTROL)) {
                // Remove from selection
                for (int i = 0; i < count; i++) {
                    toggle_particle_selection(state, particle_ids[i]);
                }
            } else {
                // New selection
                clear_selection(state);
                for (int i = 0; i < count; i++) {
                    toggle_particle_selection(state, particle_ids[i]);
                }
            }

            free(particle_ids);
            DEBUG_LOG("Selected %d particles", state->selected_count);
        }
    }
}

void draw_ui(UIState state, SimulationActor actor) {
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

    // Draw selected particles
    for (int i = 0; i < state.selected_count; i++) {
        Particle p = get_particle_state(actor->sim, state.selected_particles[i]);
        DrawCircleLines(p.position.x, p.position.y, p.size + 2, YELLOW);
    }

    // Draw "Make Static" button
    DrawRectangleRec(state.make_static_button, LIGHTGRAY);
    DrawRectangleLinesEx(state.make_static_button, 1, BLACK);
    DrawText("Make Static", state.make_static_button.x + 10, state.make_static_button.y + 5, 20, BLACK);
}
