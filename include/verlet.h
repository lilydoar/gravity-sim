#ifndef VERLET_H
#define VERLET_H

#include <stddef.h>

typedef struct Vector2D {
  double x;
  double y;
} Vector2D;

// Structure to represent a particle in 2D space
typedef struct {
  Vector2D position;          // Current position
  Vector2D previous_position; // Previous position
  Vector2D acceleration;      // Current acceleration
  double mass;                // Mass of the particle
} Particle;

// Function to initialize a particle
Particle init_particle(const Vector2D position, const Vector2D velocity,
                       double mass, double dt);

// Function to update the particle's position using Verlet integration
void verlet_step(Particle p, double dt);

// Function to compute velocity based on position updates
Vector2D compute_velocity(Particle p, double dt);

// Function to compute kinetic energy of the particle
double compute_kinetic_energy(const Particle p, double dt);

#endif // VERLET_H
