#include "simulation.h"
#include <math.h>
#include <stdlib.h>

#define BIG_G 6.67430e-11 // Gravitational constant

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

        vec2s a_g = {{0.0f, 0.0f}};

        for (uint64_t j = 0; j < simulation->particle_count; j++) {
            if (j != i) {
                Particle *other = &simulation->particles[j];
                
                vec2s r_vector = {{
                    particle->position.x - other->position.x,
                    particle->position.y - other->position.y
                }};

                double r_mag = sqrt(
                    r_vector.x * r_vector.x +
                    r_vector.y * r_vector.y
                );

                double acceleration = -BIG_G * other->mass / (r_mag * r_mag);

                vec2s r_unit_vector = {{
                    r_vector.x / r_mag,
                    r_vector.y / r_mag
                }};

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