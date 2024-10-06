#ifndef SIMULATION_H
#define SIMULATION_H

// Gravitational constant (G) in N⋅m^2/kg^2
#define GRAVITATIONAL_CONSTANT 6.67430e-11

#include "vector.h"

#include "arena_allocator.h"

#include <stdint.h>

typedef struct
{
    Vec2 position;
    Vec2 velocity;
    double mass;
} Particle;

typedef struct
{
    Particle *particles;
    uint64_t particle_count;
    double gravitational_constant;
} Simulation;

// Initialize the simulation struct
Simulation simulation_init();

// Deinitialize the simulation struct
void simulation_deinit(Simulation *simulation);

// Update the simulation
void simulation_update(Simulation *simulation, ArenaAllocator *allocator, double time_step);

#endif // SIMULATION_H