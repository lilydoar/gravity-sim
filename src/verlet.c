#include "verlet.h"
#include "cglm/types-struct.h"
#include <cglm/struct.h>

void verlet_step(vec2s *position, vec2s *previous_position, const vec2s acceleration, double dt) {
  vec2s new_position;

  new_position.x = 2.0 * position->x - previous_position->x + acceleration.x * dt * dt;
  new_position.y = 2.0 * position->y - previous_position->y + acceleration.y * dt * dt;

  *previous_position = *position;
  *position = new_position;
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
