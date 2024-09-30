#include "gravity_interactor.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define DEBUG_LOG(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__)


Action create_action(ArenaAllocator* frame_arena, ActionType type, ParticleSelection selection) {
    Action* action = arena_alloc(frame_arena, sizeof(Action));
    if (action) {
        action->type = type;
        action->selection = selection;
    }
    return *action;
    DEBUG_LOG("Completed action of type %d", action->type);
}

void apply_action(Simulation sim, Action action) {
    DEBUG_LOG("Starting action of type %d", action.type);
    if (action.type == ACTION_MAKE_STATIC) {
        if (action.selection.type == SELECTION_RECTANGLE) {
            Vector2D top_left = action.selection.shape.rectangle.top_left;
            Vector2D bottom_right = action.selection.shape.rectangle.bottom_right;
            
            // Ensure top_left is actually top-left and bottom_right is bottom-right
            Vector2D actual_top_left = {
                fmin(top_left.x, bottom_right.x),
                fmin(top_left.y, bottom_right.y)
            };
            Vector2D actual_bottom_right = {
                fmax(top_left.x, bottom_right.x),
                fmax(top_left.y, bottom_right.y)
            };
            
            int max_particles = 1000; // Adjust this value as needed
            int* particle_ids = malloc(max_particles * sizeof(int));
            
            int count = get_particles_in_rectangle(sim, actual_top_left, actual_bottom_right, particle_ids, max_particles);
            
            for (int i = 0; i < count; i++) {
                Particle p = get_particle_state(sim, particle_ids[i]);
                printf("Particle %d mode before action: %d\n", particle_ids[i], p.mode);
                p.mode = PARTICLE_MODE_STATIC;  // Set the mode to STATIC
                printf("Particle %d mode after action: %d\n", particle_ids[i], p.mode);
                set_particle_state(sim, particle_ids[i], p);
            }
            
            free(particle_ids);
        }
        // Handle other selection types if needed
    }
    // Handle other action types if needed
}

void enqueue_action(ActionQueue* queue, Action action) {
    if (queue->count < MAX_ACTIONS_PER_FRAME) {
        queue->actions[queue->count++] = action;
    }
}

Action dequeue_action(ActionQueue* queue) {
    if (queue->count > 0) {
        return queue->actions[--queue->count];
    }
    return (Action){.type = ACTION_EMPTY};
}

SimulationActor init_simulation_interactor(ArenaAllocator* app_arena) {
    SimulationActor actor = arena_alloc(app_arena, sizeof(struct SimulationActor));
    if (actor) {
        actor->arena = app_arena;
        actor->queue.count = 0;
    }
    return actor;
}

void deinit_simulation_interactor(SimulationActor actor) {
    // The memory is managed by the arena, so we don't need to free it here
    (void)actor;
}

Action query_actor(Simulation sim, SimulationActor actor) {
    // This is a placeholder implementation
    // In a real implementation, this would check for user input or other conditions
    // to determine if an action should be created
    (void)sim;
    (void)actor;
    return (Action){.type = ACTION_EMPTY};
}
