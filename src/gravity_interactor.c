#include "gravity_interactor.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *action_type_to_string(ActionType type) {
  switch (type) {
  case ACTION_MAKE_STATIC:
    return "ACTION_MAKE_STATIC";
  case ACTION_MAKE_NON_STATIC:
    return "ACTION_MAKE_NON_STATIC";
  case ACTION_REMOVE_FROM_SIM:
    return "ACTION_REMOVE_FROM_SIM";
  case ACTION_INCREASE_DENSITY:
    return "ACTION_INCREASE_DENSITY";
  case ACTION_DECREASE_DENSITY:
    return "ACTION_DECREASE_DENSITY";
  case ACTION_EMPTY:
    return "ACTION_EMPTY";
  default:
    return "UNKNOWN_ACTION";
  }
}

#define DEBUG_LOG(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__)

Action create_action(ArenaAllocator *frame_arena, ActionType type,
                     ParticleSelection selection) {
  Action action;
  action.type = type;

  if (frame_arena) {
    action.selection.particle_ids =
        arena_alloc(frame_arena, selection.count * sizeof(int));
    if (action.selection.particle_ids) {
      memcpy(action.selection.particle_ids, selection.particle_ids,
             selection.count * sizeof(int));
      action.selection.count = selection.count;
    } else {
      action.selection.count = 0;
    }
  } else {
    action.selection.particle_ids = selection.particle_ids;
    action.selection.count = selection.count;
  }

  DEBUG_LOG("Created action of type %s with %d particles",
            action_type_to_string(action.type), action.selection.count);
  return action;
}

void apply_action(Simulation sim, Action action) {
  DEBUG_LOG("Applying action of type %s", action_type_to_string(action.type));
  if (action.type == ACTION_MAKE_STATIC) {
    int modified_count = 0;
    int modified_ids[1000]; // Adjust size as needed

    for (int i = 0; i < action.selection.count; i++) {
      int particle_id = action.selection.particle_ids[i];
      Particle p = get_particle_state(sim, particle_id);
      modified_ids[modified_count++] = particle_id;
      p.mode = PARTICLE_MODE_STATIC;
      set_particle_state(sim, particle_id, p);
    }

    if (modified_count > 0) {
      DEBUG_LOG("Modified %d particles to static mode: ", modified_count);
      int max_display = 10; // Maximum number of particle IDs to display
      for (int i = 0; i < modified_count && i < max_display; i++) {
        DEBUG_LOG("%d ", modified_ids[i]);
      }
      if (modified_count > max_display) {
        DEBUG_LOG("..."); // Indicate that the list is truncated
      }
    } else {
      DEBUG_LOG("No particles were modified to static mode");
    }
  }
  // Handle other action types if needed
}

void enqueue_action(ActionQueue *queue, Action action) {
  if (queue->count < MAX_ACTIONS_PER_FRAME) {
    queue->actions[queue->count++] = action;
  }
}

Action dequeue_action(ActionQueue *queue) {
  if (queue->count > 0) {
    return queue->actions[--queue->count];
  }
  return (Action){.type = ACTION_EMPTY};
}

SimulationActor init_simulation_interactor(ArenaAllocator *app_arena,
                                           Simulation sim) {
  SimulationActor actor =
      arena_alloc(app_arena, sizeof(struct SimulationActor));
  if (actor) {
    actor->arena = app_arena;
    actor->queue.count = 0;
    actor->sim = sim;
  }
  return actor;
}

void deinit_simulation_interactor(SimulationActor actor) {
  // The memory is managed by the arena, so we don't need to free it here
  (void)actor;
}

Action query_actor(Simulation sim, SimulationActor actor) {
  // This is a placeholder implementation
  // In a real implementation, this would check for user input or other
  // conditions to determine if an action should be created
  (void)sim;
  (void)actor;
  return (Action){.type = ACTION_EMPTY};
}
