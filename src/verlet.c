#include "verlet.h"
#include "cglm/types-struct.h"
#include <cglm/struct.h>

// Function to update the particle's position using Verlet integration
void verlet_step(VerletParticle *p, double dt) {
  vec2s new_position;

  // Compute new position using Verlet integration
  new_position.x = 2.0 * p->position.x - p->previous_position.x +
                   p->acceleration.x * dt * dt;
  new_position.y = 2.0 * p->position.y - p->previous_position.y +
                   p->acceleration.y * dt * dt;

  // Update previous position
  p->previous_position = p->position;

  // Update current position
  p->position = new_position;
}

// Function to compute velocity based on position updates
vec2s compute_velocity(VerletParticle p, double dt) {
  vec2s velocity;
  velocity.x = (p.position.x - p.previous_position.x) / dt;
  velocity.y = (p.position.y - p.previous_position.y) / dt;
  return velocity;
}

vec2s compute_previus_position(const vec2s position, const vec2s velocity,
                               double dt) {
  vec2s previous_position;
  previous_position.x = position.x - velocity.x * dt;
  previous_position.y = position.y - velocity.y * dt;
  return previous_position;
}

// Function to compute kinetic energy of the particle
double compute_kinetic_energy(const VerletParticle p, double mass, double dt) {
  vec2s v = compute_velocity(p, dt);
  double speed_squared = v.x * v.x + v.y * v.y;
  return 0.5 * mass * speed_squared;
}
