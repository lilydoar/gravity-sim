#ifndef VERLET_H
#define VERLET_H

#include <cglm/struct.h>

// Structure to represent a particle in 2D space
typedef struct {
  vec2s position;          // Current position
  vec2s previous_position; // Previous position
  vec2s acceleration;      // Current acceleration
} VerletParticle;

// Function to initialize a particle
VerletParticle init_verlet_particle(const vec2s position, const vec2s velocity,
                                    double dt);

void verlet_integration(Particle *p, vec2s total_force, double time_step);
void verlet_step(VerletParticle *p, double dt);

// Function to compute velocity based on position updates
vec2s compute_velocity(VerletParticle p, double dt);

// Function to compute kinetic energy of the particle
double compute_kinetic_energy(const VerletParticle p, double mass, double dt);

#endif // VERLET_H
