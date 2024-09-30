#include "verlet.h"
#include <cglm/struct.h>

// Function to initialize a particle
VerletParticle init_particle(const vec2s position, const vec2s velocity,
                             double mass, double dt) {
  VerletParticle p;
  p.mass = mass;

  // Initialize current position
  p.position = position;

  // Initialize acceleration to zero (user can set it later)
  p.acceleration = (vec2s){0};

  // Estimate previous position using initial velocity
  p.previous_position.x = p.position.x - velocity.x * dt;
  p.previous_position.y = p.position.y - velocity.y * dt;

  return p;
}

// Function to update the particle's position using Verlet integration
void verlet_step(VerletParticle p, double dt) {
  vec2s new_position;

  // Compute new position using Verlet integration
  new_position.x =
      2.0 * p.position.x - p.previous_position.x + p.acceleration.x * dt * dt;
  new_position.y =
      2.0 * p.position.y - p.previous_position.y + p.acceleration.y * dt * dt;

  // Update previous position
  p.previous_position = p.position;

  // Update current position
  p.position = new_position;
}

// Function to compute velocity based on position updates
vec2s compute_velocity(VerletParticle p, double dt) {
  vec2s velocity;
  velocity.x = (p.position.x - p.previous_position.x) / dt;
  velocity.y = (p.position.y - p.previous_position.y) / dt;
  return velocity;
}

// Function to compute kinetic energy of the particle
double compute_kinetic_energy(const VerletParticle p, double dt) {
  vec2s v = compute_velocity(p, dt);
  double speed_squared = v.x * v.x + v.y * v.y;
  return 0.5 * p.mass * speed_squared;
}
