#include "gravity_interactor.h"
#include "gravity.h"
#include <assert.h>
#include <stdio.h>

// Mock functions for Simulation
Simulation mock_sim;
Particle mock_particles[10];

Particle get_particle_state(Simulation sim, int particle_id) {
    return mock_particles[particle_id];
}

void set_particle_state(Simulation sim, int particle_id, Particle p) {
    mock_particles[particle_id] = p;
}

int get_particles_in_rectangle(Simulation sim, Vector2D top_left, Vector2D bottom_right, int* particle_ids, int max_count) {
    // Simple mock: return 2 particles
    particle_ids[0] = 0;
    particle_ids[1] = 1;
    return 2;
}

void test_apply_action() {
    // Initialize mock particles
    for (int i = 0; i < 10; i++) {
        mock_particles[i].mode = PARTICLE_MODE_DYNAMIC;
    }

    // Create a test action
    Action action = {
        .type = ACTION_MAKE_STATIC,
        .selection = {
            .type = SELECTION_RECTANGLE,
            .shape = {
                .rectangle = {
                    .top_left = {0, 0},
                    .bottom_right = {100, 100}
                }
            }
        }
    };

    // Apply the action
    apply_action(mock_sim, action);

    // Check if particles were set to static
    assert(mock_particles[0].mode == PARTICLE_MODE_STATIC);
    assert(mock_particles[1].mode == PARTICLE_MODE_STATIC);

    printf("test_apply_action passed\n");
}

int main() {
    test_apply_action();
    return 0;
}
