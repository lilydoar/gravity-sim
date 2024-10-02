#include "unity.h"
#include "gravity_interactor.h"
#include "gravity.h"

// Mock functions for Simulation
Simulation mock_sim;
Particle mock_particles[10];

Particle get_particle_state(Simulation sim, int particle_id) {
    return mock_particles[particle_id];
}

void set_particle_state(Simulation sim, int particle_id, Particle p) {
    mock_particles[particle_id] = p;
}

int get_particles_in_rectangle(Simulation sim, vec2s top_left, vec2s bottom_right, int* particle_ids, int max_count) {
    // Simple mock: return 2 particles
    particle_ids[0] = 0;
    particle_ids[1] = 1;
    return 2;
}

// Mock function for arena allocation
void* arena_alloc(ArenaAllocator* arena, size_t size) {
    return malloc(size);
}

void test_apply_action() {
    // Initialize mock particles
    for (int i = 0; i < 10; i++) {
        mock_particles[i].mode = PARTICLE_MODE_DYNAMIC;
    }

    // Create a test action
    ParticleSelection selection = {
        .type = SELECTION_RECTANGLE,
        .shape = {
            .rectangle = {
                .top_left = {{0, 0}},
                .bottom_right = {{100, 100}}
            }
        }
    };
    
    ArenaAllocator mock_arena;
    Action action = create_action(&mock_arena, ACTION_MAKE_STATIC, selection);

    // Apply the action
    apply_action(mock_sim, action);

    // Check if particles were set to static
    TEST_ASSERT_EQUAL(PARTICLE_MODE_STATIC, mock_particles[0].mode);
    TEST_ASSERT_EQUAL(PARTICLE_MODE_STATIC, mock_particles[1].mode);
}

void setUp(void) {
    // Set up any necessary test environment here
}

void tearDown(void) {
    // Clean up any necessary test environment here
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_apply_action);
    return UNITY_END();
}

// Add these lines to satisfy Unity's requirements
void setUp(void) {}
void tearDown(void) {}
