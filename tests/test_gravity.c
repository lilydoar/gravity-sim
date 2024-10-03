#include "unity.h"
#include "gravity.h"
#include "verlet.h"

void setUp(void) {}
void tearDown(void) {}

void test_VerletParticleInitialization(void) {
    vec2s position = {{0.0, 0.0}};
    vec2s velocity = {{1.0, 1.0}};
    double dt = 0.1;
    VerletParticle particle = init_verlet_particle(position, velocity, dt);

    TEST_ASSERT_EQUAL_DOUBLE(position.x, particle.position.x);
    TEST_ASSERT_EQUAL_DOUBLE(position.y, particle.position.y);
}

void test_VerletParticlePositionUpdate(void) {
    vec2s position = {{0.0, 0.0}};
    vec2s velocity = {{1.0, 1.0}};
    double dt = 0.1;
    VerletParticle particle = init_verlet_particle(position, velocity, dt);
    particle.acceleration = (vec2s){{0.0, 0.0}};

    verlet_step(&particle, dt);

    TEST_ASSERT_EQUAL_DOUBLE(1.0, particle.position.x);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, particle.position.y);
}

void test_StaticParticleBehavior(void) {
    Particle static_particle = {
        .mode = PARTICLE_MODE_STATIC,
        .position = {{0.0, 0.0}},
        .velocity = {{0.0, 0.0}},
        .mass = 1.0,
        .size = 1.0
    };

    vec2s force = {{1.0, 1.0}};
    verlet_integration(&static_particle, force, 0.1);

    TEST_ASSERT_EQUAL_DOUBLE(0.0, static_particle.position.x);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, static_particle.position.y);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_VerletParticleInitialization);
    RUN_TEST(test_VerletParticlePositionUpdate);
    RUN_TEST(test_StaticParticleBehavior);
    return UNITY_END();
}
