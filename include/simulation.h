#ifndef SIMULATION_H
#define SIMULATION_H

#include <cglm/struct.h>

#include <stdint.h>

typedef struct Particle {
    vec2s position;
    vec2s velocity;
    double mass;
} Particle;

typedef struct Simulation {
    Particle *particles;
    uint64_t particle_count;
} Simulation;

void simulation_init(Simulation *simulation);
void simulation_deinit(Simulation *simulation);

void simulation_update(Simulation *simulation, double dt);

#endif