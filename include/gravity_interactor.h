#ifndef SIM_ACTOR_H
#define SIM_ACTOR_H

#include "gravity.h"
#include <stdbool.h>
#include <stdint.h>

// Define a structure for queuing actions on a running simulation
typedef struct {
  // TODO
} SimulationQueue;

// Enum for action types
typedef enum {
  ACTION_MAKE_STATIC,
  ACTION_MAKE_NON_STATIC,
  ACTION_REMOVE_FROM_SIM,
  ACTION_INCREASE_DENSITY,
  ACTION_DECREASE_DENSITY,
  // Add other action types as needed
} ActionType;

// Tagged union for actions
typedef struct {
  ActionType type;
  union {
    struct {
      int *particle_ids; 
      uint64_t particle_count;
    } particles;
    // Add other action parameter structs as needed
  } params;
} Action;

// Opaque type for the simulation_interactor
typedef void *SimulationActor;

/**
 * Initializes the simulation_interactor with the specified options.
 *
 * @param options A SimulationActorOptions struct containing configuration
 * settings for the simulation_interactor.
 * @return A SimulationActor handle to be used with other simulation_interactor
 * functions.
 */
SimulationActor init_simulation_interactor();

/**
 * Deinitializes the simulation_interactor, freeing any allocated resources.
 *
 * @param sim The SimulationActor handle to be deinitialized.
 */
void deinit_simulation_interactor(SimulationActor sim);

/**
 * Overview:
 * Perform the sequence of actions defined within the actor.
 * The advancement of actions is limited by the simulation.
 * An actor must be able to be called across many timesteps to perform a
 * sequence of actions. The actor can be queried to optionally produce an Action
 * that will be added to the back of a queue of actions attached to the
 * simulation.
 *
 * query_actor:
 *
 * Give simulation read access to an actor and in response the Actor will
 * optionally return an Action. TODO: Give the actor access to other
 * contexts such as user input.
 *
 * @param sim The Simulation handle to queue the action to.
 * @param actor The SimulationActor to optionally produce an action
 */
Action query_actor(Simulation sim, SimulationActor actor);

#endif // SIM_ACTOR_H
