#include "verlet.h"

// Function to initialize a particle
Particle init_particle(const Vector2D position, const Vector2D velocity,
                       double mass, double dt) {
  Particle p;
  p.mass = mass;

  // Initialize current position
  p.position = position;

  // Initialize acceleration to zero (user can set it later)
  p.acceleration = (Vector2D){0};

  // Estimate previous position using initial velocity
  p.previous_position.x = p.position.x - velocity.x * dt;
  p.previous_position.y = p.position.y - velocity.y * dt;

  return p;
}

// Function to update the particle's position using Verlet integration
void verlet_step(Particle p, double dt) {
  Vector2D new_position;

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
Vector2D compute_velocity(Particle p, double dt) {
  Vector2D velocity;
  velocity.x = (p.position.x - p.previous_position.x) / dt;
  velocity.y = (p.position.y - p.previous_position.y) / dt;
  return velocity;
}

// Function to compute kinetic energy of the particle
double compute_kinetic_energy(const Particle p, double dt) {
  Vector2D v = compute_velocity(p, dt);
  double speed_squared = v.x * v.x + v.y * v.y;
  return 0.5 * p.mass * speed_squared;
}
