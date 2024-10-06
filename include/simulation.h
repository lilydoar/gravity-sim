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

// Get the particle at the index
Particle *simulation_get_particle(Simulation *simulation, uint64_t index);

// Add a new particle to the simulation
void simulation_new_particle(Simulation *simulation, ArenaAllocator *allocator, Particle particle);

#endif // SIMULATION_H