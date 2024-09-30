#ifndef GRAVITY_H
#define GRAVITY_H

#include <stdbool.h>
#include <stdint.h>

// ... (previous code remains unchanged)

/**
 * Retrieves particles within a specified rectangular area.
 *
 * @param sim The Simulation handle.
 * @param top_left The top-left corner of the rectangle.
 * @param bottom_right The bottom-right corner of the rectangle.
 * @param particle_ids An array to store the IDs of particles found.
 * @param max_count The maximum number of particle IDs that can be stored.
 * @return The number of particles found within the rectangle.
 */
int get_particles_in_rectangle(Simulation sim, Vector2D top_left, Vector2D bottom_right, int* particle_ids, int max_count);

/**
 * Retrieves particles within a specified circular area.
 *
 * @param sim The Simulation handle.
 * @param center The center of the circle.
 * @param radius The radius of the circle.
 * @param particle_ids An array to store the IDs of particles found.
 * @param max_count The maximum number of particle IDs that can be stored.
 * @return The number of particles found within the circle.
 */
int get_particles_in_circle(Simulation sim, Vector2D center, float radius, int* particle_ids, int max_count);

// ... (rest of the file remains unchanged)

#endif // GRAVITY_H
