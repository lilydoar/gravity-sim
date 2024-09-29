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
  float mass;
  float size;
  Vector2D position;
  Vector2D velocity;
} Particle;

typedef enum { DISTRIBUTION_UNIFORM, DISTRIBUTION_NORMAL } Distribution;

typedef enum {
  VELOCITY_ZERO,
  VELOCITY_PERPENDICULAR,
  VELOCITY_TOWARDS_ORIGIN,
  VELOCITY_AWAY_FROM_ORIGIN,
  VELOCITY_RANDOM_DIRECTION
} VelocityInitMode;

typedef struct {
  double time_step;       // The fixed time delta of the simulation
  uint64_t substeps;      // The number of substeps within each step
  bool enable_collisions; // Flag to enable or disable collision handling
  uint64_t
      collision_iterations; // Number of iterations for collision resolution
  double gravitational_constant; // Gravitational constant for the simulation

  uint64_t particle_count;            // The number of starting particles
  Distribution position_distribution; // Distribution type for position
  Distribution size_distribution;     // Distribution type for size
  Distribution mass_distribution;     // Distribution type for mass

  VelocityInitMode velocity_init_mode; // Mode for initializing velocities
  Distribution velocity_magnitude_distribution; // Distribution type for
                                                // velocity magnitude

  Vector2D position_range[2]; // Two vectors defining the area for position
  float size_range[2];        // Two floats defining the range for size
  float mass_range[2];        // Two floats defining the range for mass
  Vector2D
      velocity_range; // Two doubles defining the range for velocity magnitude
} SimulationOptions;

// Opaque type for the simulation
typedef void *Simulation;

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
 * Advances the simulation by a single step.
 *
 * @param sim The Simulation handle.
 */
void step_simulation(Simulation sim);

/**
 * Toggles collision handling in the simulation.
 *
 * @param enable A boolean flag to enable or disable collision handling.
 */
void toggle_collisions(bool enable);

/**
 * Retrieves the current state of a specific particle.
 *
 * @param sim The Simulation handle.
 * @param particle_id The unique identifier of the particle.
 * @return A Particle struct containing the current state of the particle.
 */
Particle get_particle_state(Simulation sim, int particle_id);

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

/**
 * Retrieves the total number of particles in the simulation.
 *
 * @param sim The Simulation handle.
 * @return The total number of particles.
 */
uint64_t get_particle_count(Simulation sim);

/**
 * Retrieves the position range of the simulation.
 *
 * @param sim The Simulation handle.
 * @param min A pointer to store the minimum position vector.
 * @param max A pointer to store the maximum position vector.
 */
void get_position_range(Simulation sim, Vector2D *min, Vector2D *max);

#endif // GRAVITY_H
