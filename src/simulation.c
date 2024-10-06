#include "simulation.h"

#include "arena_allocator.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Forward declarations
double calculate_force_gravity(Particle p1, Particle p2,
                               double gravitational_constant);

// Initialize the simulation struct
Simulation simulation_init(double gravitational_constant) {
  return (Simulation){.gravitational_constant = gravitational_constant};
}

// Deinitialize the simulation struct
void simulation_deinit(Simulation *simulation) {
  free(simulation->particles);
  simulation->particles = NULL;
  simulation->particle_count = 0;
}

// Update the simulation
void simulation_update(Simulation *simulation, ArenaAllocator *allocator,
                       double time_step) {
  Vec2 *accelerations =
      arena_alloc(allocator, sizeof(Vec2) * simulation->particle_count);

  // Initialize accelerations to zero
  for (uint64_t i = 0; i < simulation->particle_count; i++) {
    accelerations[i] = vec2_zero();
  }

  // Calculate the acceleration for each particle
  for (uint64_t i = 0; i < simulation->particle_count; i++) {
    Particle *p1 = &simulation->particles[i];

    for (uint64_t j = i + 1; j < simulation->particle_count; j++) {
      Particle *p2 = &simulation->particles[j];

      // Calculate the gravitational force magnitude
      double force =
          calculate_force_gravity(*p1, *p2, simulation->gravitational_constant);

      // Calculate the normalized direction vector from p1 to p2
      Vec2 direction = vec2_norm(vec2_sub(p2->position, p1->position));

      // Calculate accelerations
      Vec2 acceleration_p1 = vec2_scale(direction, force / p1->mass);
      Vec2 acceleration_p2 = vec2_scale(direction, -force / p2->mass);

      // Update accelerations
      accelerations[i] = vec2_add(accelerations[i], acceleration_p1);
      accelerations[j] = vec2_add(accelerations[j], acceleration_p2);
    }
  }

  // Update the velocity and position of each particle
  for (uint64_t i = 0; i < simulation->particle_count; i++) {
    Particle *p = &simulation->particles[i];
    p->velocity =
        vec2_add(p->velocity, vec2_scale(accelerations[i], time_step));
    p->position = vec2_add(p->position, vec2_scale(p->velocity, time_step));
  }

  // Resolve collisions for each particle
  for (uint64_t i = 0; i < simulation->particle_count; i++) {
    Particle *p1 = &simulation->particles[i];
    for (uint64_t j = i + 1; j < simulation->particle_count; j++) {
      Particle *p2 = &simulation->particles[j];

      Vec2 diff = vec2_sub(p1->position, p2->position);
      double distance = vec2_len(diff);
      double radius_sum = p1->radius + p2->radius;

      if (distance < radius_sum) {
        Vec2 collision_normal = vec2_norm(diff);
        double overlap = radius_sum - distance;
        double total_inverse_mass = (1.0 / p1->mass) + (1.0 / p2->mass);

        // Corrected separation calculation
        Vec2 separation =
            vec2_scale(collision_normal, overlap / total_inverse_mass);

        // Corrected position adjustments
        p1->position =
            vec2_add(p1->position, vec2_scale(separation, 1.0 / p1->mass));
        p2->position =
            vec2_sub(p2->position, vec2_scale(separation, 1.0 / p2->mass));

        Vec2 relative_velocity = vec2_sub(p1->velocity, p2->velocity);
        double normal_velocity = vec2_dot(relative_velocity, collision_normal);

        if (normal_velocity > 0) {
          continue;
        }

        double restitution = 1.0;
        double impulse_scalar =
            -(1 + restitution) * normal_velocity / total_inverse_mass;
        Vec2 impulse = vec2_scale(collision_normal, impulse_scalar);

        // Corrected velocity adjustments
        p1->velocity =
            vec2_add(p1->velocity, vec2_scale(impulse, 1.0 / p1->mass));
        p2->velocity =
            vec2_sub(p2->velocity, vec2_scale(impulse, 1.0 / p2->mass));
      }
    }
  }
}

// Get the particle at the index
Particle *simulation_get_particle(Simulation *simulation, uint64_t index) {
  return &simulation->particles[index];
}

// Add a new particle to the simulation
void simulation_new_particle(Simulation *simulation, ArenaAllocator *allocator,
                             Particle particle) {
  assert(simulation);
  assert(allocator);

  Particle *new_particle = arena_alloc(allocator, sizeof(Particle));
  *new_particle = particle;
  simulation->particle_count++;

  if (simulation->particles == NULL) {
    simulation->particles = new_particle;
  }
}

// Calculate the force between two particles
double calculate_force_gravity(Particle p1, Particle p2,
                               double gravitational_constant) {
  double distance_squared = vec2_dist_squared(p1.position, p2.position);
  double force =
      (gravitational_constant * p1.mass * p2.mass) / distance_squared;
  return force;
}