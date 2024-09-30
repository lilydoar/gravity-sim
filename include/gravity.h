#ifndef GRAVITY_H
#define GRAVITY_H

#include <stdbool.h>
#include <stdint.h>

#include <cglm/struct.h>

typedef enum {
  PARTICLE_MODE_STATIC,
  PARTICLE_MODE_DYNAMIC,
  PARTICLE_MODE_VERLET
  // Add more modes here as needed
} ParticleMode;

typedef struct Particle {
  ParticleMode mode;
  vec2s position;
  vec2s velocity;
  double mass;
  double size;
} Particle;

typedef struct Simulation *Simulation;

typedef enum { DISTRIBUTION_UNIFORM, DISTRIBUTION_NORMAL } DistributionType;

typedef struct {
  DistributionType type;
  union {
    struct {
      double min;
      double max;
    } uniform;
    struct {
      double mean;
      double stddev;
    } normal;
  } params;
} Distribution;

typedef enum {
  VELOCITY_INIT_ZERO,
  VELOCITY_INIT_RANDOM,
  VELOCITY_INIT_PERPENDICULAR_TO_ORIGIN,
  VELOCITY_INIT_TOWARDS_ORIGIN,
  VELOCITY_INIT_AWAY_FROM_ORIGIN
} VelocityInitMode;

typedef struct {
  double time_step;
  uint64_t substeps;
  bool enable_collisions;
  uint64_t collision_iterations;
  double gravitational_constant;
  uint64_t particle_count;
  Distribution position_x_distribution;
  Distribution position_y_distribution;
  Distribution mass_distribution;
  Distribution size_distribution;
  ParticleMode initial_particle_mode;
  VelocityInitMode velocity_init_mode;
  Distribution velocity_magnitude_distribution;
} SimulationOptions;

// ... (previous code remains unchanged)

/**
 * Retrieves particles within a specified rectangular area.
 *
 * @param sim The Simulation handle.
 * @param top_left The top-left corner of the rectangle.
 * @param bottom_right The bottom-right corner of the rectangle.
 * @param particle_ids An array to store the IDs of particles found.
 * @param max_count The maximum number of particle IDs that can be stored.
 * @return The number of particles found within the rectangle.
 */
int get_particles_in_rectangle(Simulation sim, vec2s top_left,
                               vec2s bottom_right, int *particle_ids,
                               int max_count);

/**
 * Retrieves particles within a specified circular area.
 *
 * @param sim The Simulation handle.
 * @param center The center of the circle.
 * @param radius The radius of the circle.
 * @param particle_ids An array to store the IDs of particles found.
 * @param max_count The maximum number of particle IDs that can be stored.
 * @return The number of particles found within the circle.
 */
int get_particles_in_circle(Simulation sim, vec2s center, float radius,
                            int *particle_ids, int max_count);

// New function declarations
void get_position_range(Simulation sim, vec2s *min, vec2s *max);
uint64_t get_particle_count(Simulation sim);
Particle get_particle_state(Simulation sim, int particle_id);
void set_particle_state(Simulation sim, int particle_id, Particle p);

// ... (rest of the file remains unchanged)

#endif // GRAVITY_H
