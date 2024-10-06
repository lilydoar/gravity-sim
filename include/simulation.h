#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdint.h>

#include "math/vector.h"

typedef struct Particle
{
    Vec2 position;
    Vec2 velocity;
    double mass;
} Particle;

typedef struct Simulation
{
    Particle *particles;
    uint64_t particle_count;
} Simulation;

void simulation_init(Simulation *simulation);
void simulation_deinit(Simulation *simulation);

void simulation_update(Simulation *simulation, double dt);

#endif