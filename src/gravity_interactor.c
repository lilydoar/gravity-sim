#include "gravity_interactor.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const char* action_type_to_string(ActionType type) {
    switch (type) {
        case ACTION_MAKE_STATIC: return "ACTION_MAKE_STATIC";
        case ACTION_MAKE_NON_STATIC: return "ACTION_MAKE_NON_STATIC";
        case ACTION_REMOVE_FROM_SIM: return "ACTION_REMOVE_FROM_SIM";
        case ACTION_INCREASE_DENSITY: return "ACTION_INCREASE_DENSITY";
        case ACTION_DECREASE_DENSITY: return "ACTION_DECREASE_DENSITY";
        case ACTION_EMPTY: return "ACTION_EMPTY";
        default: return "UNKNOWN_ACTION";
    }
}

#define DEBUG_LOG(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__)

Action create_action(ArenaAllocator *frame_arena, ActionType type,
                     ParticleSelection selection) {
  Action action;
  action.type = type;
  action.selection = selection;
  
  if (frame_arena) {
    Action *action_ptr = arena_alloc(frame_arena, sizeof(Action));
    if (action_ptr) {
      *action_ptr = action;
      DEBUG_LOG("Created action of type %s", action_type_to_string(action_ptr->type));
      return *action_ptr;
    }
  }
  
  DEBUG_LOG("Created action of type %s", action_type_to_string(action.type));
  return action;
}

void apply_action(Simulation sim, Action action) {
  DEBUG_LOG("Applying action of type %s", action_type_to_string(action.type));
  if (action.type == ACTION_MAKE_STATIC) {
    if (action.selection.type == SELECTION_RECTANGLE) {
      vec2s top_left = action.selection.shape.rectangle.top_left;
      vec2s bottom_right = action.selection.shape.rectangle.bottom_right;

      // Ensure top_left is actually top-left and bottom_right is bottom-right
      vec2s actual_top_left = {
          {fmin(top_left.x, bottom_right.x), fmin(top_left.y, bottom_right.y)}};
      vec2s actual_bottom_right = {
          {fmax(top_left.x, bottom_right.x), fmax(top_left.y, bottom_right.y)}};

      int max_particles = 1000; // Adjust this value as needed
      int *particle_ids = malloc(max_particles * sizeof(int));

      int count =
          get_particles_in_rectangle(sim, actual_top_left, actual_bottom_right,
                                     particle_ids, max_particles);

      int modified_count = 0;
      int modified_ids[1000]; // Adjust size as needed

      for (int i = 0; i < count; i++) {
        Particle p = get_particle_state(sim, particle_ids[i]);
        if (p.mode != PARTICLE_MODE_STATIC) {
          modified_ids[modified_count++] = particle_ids[i];
          p.mode = PARTICLE_MODE_STATIC;
          set_particle_state(sim, particle_ids[i], p);
        }
      }

      if (modified_count > 5) {
        printf("Modified %d particles to static mode: ", modified_count);
        int max_display = 10; // Maximum number of particle IDs to display
        for (int i = 0; i < modified_count && i < max_display; i++) {
          printf("%d ", modified_ids[i]);
        }
        if (modified_count > max_display) {
          printf("..."); // Indicate that the list is truncated
        }
        printf("\n");
      } else {
        for (int i = 0; i < modified_count; i++) {
          printf("Particle %d mode changed to static\n", modified_ids[i]);
        }
      }

      free(particle_ids);
    }
    // Handle other selection types if needed
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

SimulationActor init_simulation_interactor(ArenaAllocator *app_arena, Simulation sim) {
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
