#include "verlet.h"

Vec2 verlet_step(Vec2 position, Vec2 previous_position, Vec2 acceleration,
                  double dt) {
  Vec2 new_position;
  new_position.x =
      2.0 * position.x - previous_position.x + acceleration.x * dt * dt;
  new_position.y =
      2.0 * position.y - previous_position.y + acceleration.y * dt * dt;
  return new_position;
}

// Function to compute velocity based on position updates
Vec2 compute_velocity(const Vec2 position, const Vec2 previous_position,
                       double dt) {
  Vec2 velocity;
  velocity.x = (position.x - previous_position.x) / dt;
  velocity.y = (position.y - previous_position.y) / dt;
  return velocity;
}

Vec2 compute_previus_position(const Vec2 position, const Vec2 velocity,
                               double dt) {
  Vec2 previous_position;
  previous_position.x = position.x - velocity.x * dt;
  previous_position.y = position.y - velocity.y * dt;
  return previous_position;
}

// Function to compute kinetic energy of the particle
double compute_kinetic_energy(const Vec2 position,
                              const Vec2 previous_position, double mass,
                              double dt) {
  Vec2 v = compute_velocity(position, previous_position, dt);
  double speed_squared = v.x * v.x + v.y * v.y;
  return 0.5 * mass * speed_squared;
}
