#ifndef GRAVITY_C
#define GRAVITY_C

#include "gravity.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

// Define the concrete Simulation struct
typedef struct {
  Particle *particles;     // Array of particles
  uint64_t particle_count; // Number of particles
  bool enable_collisions;  // Collision handling flag
  double time_step;        // Time step for the simulation
  uint64_t substeps;       // Number of substeps
} SimulationStruct;

double random_uniform_double(double min, double max) {
  return min + (double)rand() / RAND_MAX * (max - min);
}

float random_uniform_float(float min, float max) {
  return min + (float)rand() / RAND_MAX * (max - min);
}

double random_normal_double(double mean, double stddev) {
  double u1 = (double)rand() / RAND_MAX;
  double u2 = (double)rand() / RAND_MAX;
  double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
  return z0 * stddev + mean;
}

float random_normal_float(float mean, float stddev) {
  float u1 = (float)rand() / RAND_MAX;
  float u2 = (float)rand() / RAND_MAX;
  float z0 = sqrtf(-2.0f * logf(u1)) * cosf(2.0f * M_PI * u2);
  return z0 * stddev + mean;
}

Simulation init_simulation(SimulationOptions options) {
  // Allocate memory for the simulation
  SimulationStruct *sim = (SimulationStruct *)malloc(sizeof(SimulationStruct));
  if (!sim) {
    return NULL; // Allocation failed
  }

  // Initialize the simulation with the given options
  sim->particle_count = options.particle_count;
  sim->enable_collisions = options.enable_collisions;
  sim->time_step = options.time_step;
  sim->substeps = options.substeps;

  // Allocate memory for particles
  sim->particles = (Particle *)malloc(sizeof(Particle) * sim->particle_count);
  if (!sim->particles) {
    free(sim);
    return NULL; // Allocation failed
  }

  // Initialize particles based on the specified distributions
  for (uint64_t i = 0; i < sim->particle_count; ++i) {
    // Initialize position
    switch (options.position_distribution) {
    case DISTRIBUTION_UNDEFINED:
    case DISTRIBUTION_UNIFORM:
      sim->particles[i].position.x = random_uniform_double(
          options.position_range[0].x, options.position_range[1].x);
      sim->particles[i].position.y = random_uniform_double(
          options.position_range[0].y, options.position_range[1].y);
      break;
    case DISTRIBUTION_NORMAL:
      sim->particles[i].position.x = random_normal_double(
          (options.position_range[0].x + options.position_range[1].x) / 2,
          (options.position_range[1].x - options.position_range[0].x) / 6);
      sim->particles[i].position.y = random_normal_double(
          (options.position_range[0].y + options.position_range[1].y) / 2,
          (options.position_range[1].y - options.position_range[0].y) / 6);
      break;
    default:
      break;
    }

    // Initialize size
    switch (options.size_distribution) {
    case DISTRIBUTION_UNDEFINED:
    case DISTRIBUTION_UNIFORM:
      sim->particles[i].size =
          random_uniform_float(options.size_range[0], options.size_range[1]);
      break;
    case DISTRIBUTION_NORMAL:
      sim->particles[i].size = random_normal_float(
          (options.size_range[0] + options.size_range[1]) / 2,
          (options.size_range[1] - options.size_range[0]) / 6);
      break;
    default:
      break;
    }

    // Initialize mass
    switch (options.mass_distribution) {
    case DISTRIBUTION_UNDEFINED:
    case DISTRIBUTION_UNIFORM:
      sim->particles[i].mass =
          random_uniform_float(options.mass_range[0], options.mass_range[1]);
      break;
    case DISTRIBUTION_NORMAL:
      sim->particles[i].mass = random_normal_float(
          (options.mass_range[0] + options.mass_range[1]) / 2,
          (options.mass_range[1] - options.mass_range[0]) / 6);
      break;
    default:
      break;
    }

    // Initialize velocity
    sim->particles[i].velocity.x = 0.0; // Initial x velocity
    sim->particles[i].velocity.y = 0.0; // Initial y velocity
  }

  return (Simulation)sim;
}

void deinit_simulation(Simulation sim) {
  SimulationStruct *sim_struct = (SimulationStruct *)sim;
  if (sim_struct) {
    // Free the particles array
    free(sim_struct->particles);
    // Free the simulation struct
    free(sim_struct);
  }
}

void step_simulation(Simulation sim) {
  // Update the simulation by the given time step
}

void toggle_collisions(bool enable) {
  // Enable or disable collision handling
}

Particle get_particle_state(Simulation sim, int particle_id) {
  SimulationStruct *sim_struct = (SimulationStruct *)sim;
  if (particle_id < 0 || particle_id >= sim_struct->particle_count) {
    // Handle invalid particle_id
    Particle empty_particle = {0};
    return empty_particle;
  }
  return sim_struct->particles[particle_id];
}

int get_particles_by_area(Vector2D top_left, Vector2D bottom_right,
                          ParticleInfo *buffer, int max_count) {
  // Retrieve particles within the specified area
  return 0; // Placeholder return
}

// Adds a new particle to the simulation.
void add_particle(double mass, double size, Vector2D position,
                  Vector2D velocity) {
  // Add a new particle with specified properties
}

// Removes a particle from the simulation.
void remove_particle(int particle_id) {
  // Remove the particle with the given ID
}

// Applies an external force to a specific particle.
void apply_force(int particle_id, Vector2D force) {
  // Apply the specified force to the particle
}
void get_position_range(Simulation sim, Vector2D *min, Vector2D *max) {
  SimulationStruct *sim_struct = (SimulationStruct *)sim;
  if (sim_struct->particle_count == 0) {
    min->x = min->y = max->x = max->y = 0.0;
    return;
  }

  min->x = max->x = sim_struct->particles[0].position.x;
  min->y = max->y = sim_struct->particles[0].position.y;

  for (uint64_t i = 1; i < sim_struct->particle_count; ++i) {
    Particle p = sim_struct->particles[i];
    if (p.position.x < min->x) min->x = p.position.x;
    if (p.position.x > max->x) max->x = p.position.x;
    if (p.position.y < min->y) min->y = p.position.y;
    if (p.position.y > max->y) max->y = p.position.y;
  }
}

#endif // GRAVITY_C
