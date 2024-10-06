#include "simulation.h"

#include "math/vector.h"

#include <math.h>
#include <stdlib.h>

#define GRAVITATIONAL_CONSTANT 6.67430e-11 // Gravitational constant

void simulation_init(Simulation *simulation) {
    simulation->particles = NULL;
    simulation->particle_count = 0;
}

void simulation_deinit(Simulation *simulation) {
    free(simulation->particles);
}

void simulation_update(Simulation *simulation, double dt) {
    for (uint64_t i = 0; i < simulation->particle_count; i++) {
        Particle *particle = &simulation->particles[i];

        Vec2 a_g = vec2_zero();

        for (uint64_t j = 0; j < simulation->particle_count; j++) {
            if (j != i) {
                Particle *other = &simulation->particles[j];

                Vec2 r_vector = vec2_sub(particle->position, other->position);

                double r_mag = sqrt(
                    r_vector.x * r_vector.x +
                    r_vector.y * r_vector.y
                );

                double acceleration = -GRAVITATIONAL_CONSTANT * other->mass / (r_mag * r_mag);

                Vec2 r_unit_vector = vec2_div(r_vector, r_mag);

                a_g.x += acceleration * r_unit_vector.x;
                a_g.y += acceleration * r_unit_vector.y;
            }
        }

        particle->velocity.x += a_g.x * dt;
        particle->velocity.y += a_g.y * dt;
    }

    for (uint64_t i = 0; i < simulation->particle_count; i++) {
        Particle *particle = &simulation->particles[i];
        particle->position.x += particle->velocity.x * dt;
        particle->position.y += particle->velocity.y * dt;
    }
}