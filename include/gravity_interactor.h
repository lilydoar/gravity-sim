#ifndef GRAVITY_INTERACTOR_H
#define GRAVITY_INTERACTOR_H

#include "arena_allocator.h"
#include "gravity.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum { SELECTION_RECTANGLE, SELECTION_CIRCLE } SelectionType;

typedef struct {
  SelectionType type;
  union {
    struct {
      vec2s top_left;
      vec2s bottom_right;
    } rectangle;
    struct {
      vec2s center;
      float radius;
    } circle;
  } shape;
  int* particle_ids;
  int count;
} ParticleSelection;

typedef enum {
  ACTION_MAKE_STATIC,
  ACTION_MAKE_NON_STATIC,
  ACTION_REMOVE_FROM_SIM,
  ACTION_INCREASE_DENSITY,
  ACTION_DECREASE_DENSITY,
  ACTION_EMPTY
} ActionType;

typedef struct {
  ActionType type;
  ParticleSelection selection;
} Action;

Action create_action(ArenaAllocator *frame_arena, ActionType type,
                     ParticleSelection selection);
void apply_action(Simulation sim, Action action);

#define MAX_ACTIONS_PER_FRAME 10

typedef struct {
  Action actions[MAX_ACTIONS_PER_FRAME];
  int count;
} ActionQueue;

void enqueue_action(ActionQueue *queue, Action action);
Action dequeue_action(ActionQueue *queue);

// Define the SimulationActor struct
typedef struct SimulationActor {
  ActionQueue queue;
  ArenaAllocator *arena;
  Simulation sim;
} *SimulationActor;

SimulationActor init_simulation_interactor(ArenaAllocator *app_arena);
void deinit_simulation_interactor(SimulationActor actor);
Action query_actor(Simulation sim, SimulationActor actor);

#endif // GRAVITY_INTERACTOR_H
