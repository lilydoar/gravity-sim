#ifndef VERLET_H
#define VERLET_H

#include "gravity.h"
#include <cglm/struct.h>

// Structure to represent a particle in 2D space
typedef struct {
  vec2s position;          // Current position
  vec2s previous_position; // Previous position
  vec2s acceleration;      // Current acceleration
} VerletParticle;

// Function to initialize a particle
/*VerletParticle init_verlet_particle(const vec2s position, const vec2s
 * velocity,*/
/*                                    double dt);*/

void verlet_step(VerletParticle *p, double dt);

vec2s compute_velocity(VerletParticle p, double dt);
vec2s compute_previus_position(const vec2s position, const vec2s velocity,
                               double dt);
double compute_kinetic_energy(const VerletParticle p, double mass, double dt);

#endif // VERLET_H
