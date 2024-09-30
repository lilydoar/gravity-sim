#include "gravity_interactor.h"
#include <stdlib.h>


Action create_action(ArenaAllocator* frame_arena, ActionType type, ParticleSelection selection) {
    Action* action = arena_alloc(frame_arena, sizeof(Action));
    if (action) {
        action->type = type;
        action->selection = selection;
    }
    return *action;
}

void apply_action(Simulation sim, Action action) {
    // Implementation depends on the specific actions and how they affect the simulation
    // This is a placeholder implementation
    (void)sim;
    (void)action;
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
