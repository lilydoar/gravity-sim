#ifndef VERLET_H
#define VERLET_H

#include "gravity.h"
#include "vector.h"

Vec2 verlet_step(Vec2 position, Vec2 previous_position, Vec2 acceleration, double dt);
Vec2 compute_velocity(const Vec2 position, const Vec2 previous_position, double dt);
Vec2 compute_previus_position(const Vec2 position, const Vec2 velocity, double dt);
double compute_kinetic_energy(const Vec2 position, const Vec2 previous_position, double mass, double dt);

#endif // VERLET_H
