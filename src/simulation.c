#include "simulation.h"

#include "arena_allocator.h"

#include <assert.h>
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

  // Calculate the acceleration for each particle
  for (uint64_t i = 0; i < simulation->particle_count; i++) {
    Particle *p1 = &simulation->particles[i];

    for (uint64_t j = 0; j < simulation->particle_count; j++) {
      Particle *p2 = &simulation->particles[j];
      if (i == j)
        continue;

      double force =
          calculate_force_gravity(*p1, *p2, simulation->gravitational_constant);
      Vec2 force_vector = vec2_norm(vec2_sub(p2->position, p1->position));
      accelerations[i] =
          vec2_add(accelerations[i], vec2_scale(force_vector, force));
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
      if (i == j)
        continue;

      double distance = vec2_dist(p1->position, p2->position);
      if (distance < p1->radius + p2->radius) {
        Vec2 overlap =
            vec2_scale(vec2_norm(vec2_sub(p1->position, p2->position)),
                       p1->radius + p2->radius - distance);
        // Calculate the new position of each particle
        p1->position = vec2_add(p1->position, vec2_scale(overlap, 0.5));
        p2->position = vec2_sub(p2->position, vec2_scale(overlap, 0.5));

        // Calculate the new velocity of each particle
        Vec2 normal = vec2_norm(vec2_sub(p1->position, p2->position));
        Vec2 relative_velocity = vec2_sub(p1->velocity, p2->velocity);
        double relative_velocity_normal = vec2_dot(relative_velocity, normal);
        
        // Calculate the impulse magnitude
        double impulse_magnitude = (2 * relative_velocity_normal) / (1/p1->mass + 1/p2->mass);
        Vec2 impulse = vec2_scale(normal, impulse_magnitude);
        
        // Apply impulse to each particle, inversely proportional to their mass
        p1->velocity = vec2_sub(p1->velocity, vec2_scale(impulse, 1 / p1->mass));
        p2->velocity = vec2_add(p2->velocity, vec2_scale(impulse, 1 / p2->mass));
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