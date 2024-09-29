#ifndef GRAVITY_H
#define GRAVITY_H

#include <stdbool.h>
#include <stdint.h>

// Define a structure for a 2D vector
typedef struct {
  double x;
  double y;
} Vector2D;

// Define a structure for a particle
typedef struct {
  double mass;
  double size;
  Vector2D position;
  Vector2D velocity;
} Particle;

typedef enum {
  DISTRIBUTION_UNDEFINED,
  DISTRIBUTION_UNIFORM,
  DISTRIBUTION_NORMAL
} ParticleDistribution;
typedef struct {
  bool enable_collisions; // Flag to enable or disable collision handling
  int64_t particle_count; // The number of starting particles
  ParticleDistribution distribution; // Distribution type for spawning particles
} SimulationOptions;

// Opaque type for the simulation
typedef void* Simulation;

/**
 * Initializes the simulation with the specified options.
 *
 * @param options A SimulationOptions struct containing configuration settings
 * for the simulation.
 * @return A Simulation handle to be used with other simulation functions.
 */
Simulation init_simulation(SimulationOptions options);

/**
 * Deinitializes the simulation, freeing any allocated resources.
 *
 * @param sim The Simulation handle to be deinitialized.
 */
void deinit_simulation(Simulation sim);

/**
 * Advances the simulation by a specified time step.
 *
 * @param sim The Simulation handle.
 * @param time_step The time step by which to update the simulation.
 */
void step_simulation(Simulation sim, double time_step);

/**
 * Initializes the simulation with the specified options.
 *
 * @param options A SimulationOptions struct containing configuration settings
 * for the simulation.
 */
void init_simulation(SimulationOptions options);

/**
 * Adds a new particle to the simulation.
 *
 * @param mass The mass of the particle.
 * @param size The size (radius) of the particle.
 * @param position The initial position of the particle as a Vector2D.
 * @param velocity The initial velocity of the particle as a Vector2D.
 */
void add_particle(double mass, double size, Vector2D position,
                  Vector2D velocity);

/**
 * Removes a particle from the simulation.
 *
 * @param particle_id The unique identifier of the particle to be removed.
 */
void remove_particle(int particle_id);

/**
 * Applies an external force to a specific particle.
 *
 * @param particle_id The unique identifier of the particle.
 * @param force The force to be applied as a Vector2D.
 */
void apply_force(int particle_id, Vector2D force);

/**
 * Advances the simulation by a specified time step.
 *
 * @param time_step The time step by which to update the simulation.
 */
void update_simulation(double time_step);

/**
 * Toggles collision handling in the simulation.
 *
 * @param enable A boolean flag to enable or disable collision handling.
 */
void toggle_collisions(bool enable);

/**
 * Retrieves the current state of a specific particle.
 *
 * @param particle_id The unique identifier of the particle.
 * @return A Particle struct containing the current state of the particle.
 */
Particle get_particle_state(int particle_id);

// Structure to hold particle information
typedef struct {
  int particle_id;   // Unique identifier for the particle
  Particle particle; // The particle's data
} ParticleInfo;

/**
 * Retrieves particles within a specified rectangular area.
 *
 * @param top_left The top-left corner of the rectangle.
 * @param bottom_right The bottom-right corner of the rectangle.
 * @param buffer A pointer to an array of ParticleInfo to store the results.
 * @param max_count The maximum number of ParticleInfo entries that the buffer
 * can hold.
 * @return The number of particles found and stored in the buffer, or -1 if the
 * buffer is insufficient.
 *
 * Note: If the function returns -1, the buffer may not be in a valid state, and
 * not all particles in the specified area could be retrieved. Ensure the buffer
 * is large enough to hold all potential particles in the area for accurate
 * results.
 */
int get_particles_by_area(Vector2D top_left, Vector2D bottom_right,
                          ParticleInfo *buffer, int max_count);

#endif // GRAVITY_H
