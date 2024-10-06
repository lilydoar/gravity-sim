#ifndef VERLET_H
#define VERLET_H

#include "gravity.h"
#include <cglm/struct.h>

vec2s verlet_step(vec2s position, vec2s previous_position, vec2s acceleration, double dt);
vec2s compute_velocity(const vec2s position, const vec2s previous_position, double dt);
vec2s compute_previus_position(const vec2s position, const vec2s velocity, double dt);
double compute_kinetic_energy(const vec2s position, const vec2s previous_position, double mass, double dt);

#endif // VERLET_H
