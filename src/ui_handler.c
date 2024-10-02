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

    DEBUG_LOG("Mouse position: (%f, %f)", mouse_pos.x, mouse_pos.y);
    DEBUG_LOG("World mouse position: (%f, %f)", world_mouse_pos.x, world_mouse_pos.y);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        DEBUG_LOG("Left mouse button pressed");
        if (CheckCollisionPointRec(mouse_pos, state->make_static_button)) {
            DEBUG_LOG("Make static button clicked");
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
            DEBUG_LOG("Started rectangle selection");
        }
    } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        DEBUG_LOG("Right mouse button pressed");
        state->is_selecting = true;
        state->start_pos = world_mouse_pos;
        state->current_selection_type = SELECTION_CIRCLE;
        DEBUG_LOG("Started circle selection");
    }

    if (state->is_selecting) {
        state->current_pos = world_mouse_pos;
        DEBUG_LOG("Selecting: current position (%f, %f)", state->current_pos.x, state->current_pos.y);
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            DEBUG_LOG("Mouse button released, ending selection");
            state->is_selecting = false;

            ParticleSelection selection;
            selection.type = state->current_selection_type;

            if (selection.type == SELECTION_RECTANGLE) {
                selection.shape.rectangle.top_left = (vec2s){{fmin(state->start_pos.x, state->current_pos.x), fmin(state->start_pos.y, state->current_pos.y)}};
                selection.shape.rectangle.bottom_right = (vec2s){{fmax(state->start_pos.x, state->current_pos.x), fmax(state->start_pos.y, state->current_pos.y)}};
                DEBUG_LOG("Rectangle selection: (%f, %f) to (%f, %f)", 
                    selection.shape.rectangle.top_left.x, selection.shape.rectangle.top_left.y,
                    selection.shape.rectangle.bottom_right.x, selection.shape.rectangle.bottom_right.y);
            } else {
                selection.shape.circle.center = (vec2s){{state->start_pos.x, state->start_pos.y}};
                selection.shape.circle.radius = Vector2Distance(state->start_pos, state->current_pos);
                DEBUG_LOG("Circle selection: center (%f, %f), radius %f", 
                    selection.shape.circle.center.x, selection.shape.circle.center.y,
                    selection.shape.circle.radius);
            }

            int max_particles = 1000;
            int* particle_ids = malloc(max_particles * sizeof(int));
            if (particle_ids == NULL) {
                DEBUG_LOG("Failed to allocate memory for particle_ids");
                return;
            }
            int count;

            if (selection.type == SELECTION_RECTANGLE) {
                count = get_particles_in_rectangle(actor->sim, selection.shape.rectangle.top_left, selection.shape.rectangle.bottom_right, particle_ids, max_particles);
            } else {
                count = get_particles_in_circle(actor->sim, selection.shape.circle.center, selection.shape.circle.radius, particle_ids, max_particles);
            }
            DEBUG_LOG("Found %d particles in selection", count);

            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                DEBUG_LOG("Adding to selection");
                // Add to selection
                for (int i = 0; i < count; i++) {
                    toggle_particle_selection(state, particle_ids[i]);
                }
            } else if (IsKeyDown(KEY_LEFT_CONTROL)) {
                DEBUG_LOG("Removing from selection");
                // Remove from selection
                for (int i = 0; i < count; i++) {
                    toggle_particle_selection(state, particle_ids[i]);
                }
            } else {
                DEBUG_LOG("New selection");
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
    DEBUG_LOG("Drawing UI");
    if (state.is_selecting) {
        DEBUG_LOG("Drawing selection");
        if (state.current_selection_type == SELECTION_RECTANGLE) {
            float x = fmin(state.start_pos.x, state.current_pos.x);
            float y = fmin(state.start_pos.y, state.current_pos.y);
            float width = fabs(state.current_pos.x - state.start_pos.x);
            float height = fabs(state.current_pos.y - state.start_pos.y);
            DrawRectangleLines((int)x, (int)y, (int)width, (int)height, RED);
            DEBUG_LOG("Drawing rectangle: (%f, %f, %f, %f)", x, y, width, height);
        } else {
            float radius = Vector2Distance(state.start_pos, state.current_pos);
            DrawCircleLines((int)state.start_pos.x, (int)state.start_pos.y, radius, RED);
            DEBUG_LOG("Drawing circle: center (%f, %f), radius %f", state.start_pos.x, state.start_pos.y, radius);
        }
    }

    // Draw selected particles
    DEBUG_LOG("Drawing %d selected particles", state.selected_count);
    for (int i = 0; i < state.selected_count; i++) {
        Particle p = get_particle_state(actor->sim, state.selected_particles[i]);
        DrawCircleLines((int)p.position.x, (int)p.position.y, p.size + 2, YELLOW);
    }

    // Draw "Make Static" button
    DrawRectangleRec(state.make_static_button, LIGHTGRAY);
    DrawRectangleLinesEx(state.make_static_button, 1, BLACK);
    DrawText("Make Static", (int)state.make_static_button.x + 10, (int)state.make_static_button.y + 5, 20, BLACK);
    DEBUG_LOG("Drew Make Static button");
}
