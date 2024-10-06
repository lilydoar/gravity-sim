#ifndef GRAVITY_H
#define GRAVITY_H

#include "arena_allocator.h"
#include "iterator.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Newtons * Meters squared / Kilograms squared
#define GRAVITATIONAL_CONSTANT 6.67430e-11

typedef enum {
  PARTICLE_MODE_STATIC,
  PARTICLE_MODE_VERLET
  // ...
} ParticleMode;

typedef struct {
  ParticleMode mode;
  union {
    struct {
      Vec2 position;
      double mass;
      double radius;
    } STATIC;
    struct {
      Vec2 position;
      Vec2 position_previous;
      Vec2 acceleration;
      double mass;
      double radius;
    } VERLET;
  } params;
} SimulationParticle;

typedef struct {
  // TODO: Set of optionals paramters for modifying groups of particles
  // without overwriting all values
} SimulationParticleDiff;

typedef struct Particle {
  ParticleMode mode;
  Vec2 position;
  Vec2 velocity;
  double mass;
  double size;
} Particle;

typedef struct Simulation *Simulation;

// TODO: Extract to common file.
// TODO: https://en.wikipedia.org/wiki/List_of_probability_distributions
typedef enum {
  DISTRIBUTION_UNIFORM,
  DISTRIBUTION_NORMAL,
  DISTRIBUTION_POWER_LAW, // TODO
  DISTRIBUTION_FORCE_LAW  // TODO
} DistributionType;

typedef struct {
  DistributionType type;
  union {
    struct {
      double min;
      double max;
    } uniform;
    struct {
      double mean;
      double stddev;
    } normal;
  } params;
} Distribution;

typedef enum {
  VELOCITY_INIT_ZERO,
  VELOCITY_INIT_RANDOM,
  VELOCITY_INIT_PERPENDICULAR_TO_ORIGIN,
  VELOCITY_INIT_TOWARDS_ORIGIN,
  VELOCITY_INIT_AWAY_FROM_ORIGIN
} VelocityInitMode;

// TODO: init to an empty universe
// Use actions to add particles
typedef struct {
  double time_step;
  uint64_t substeps;
  double gravitational_constant;
} SimulationOptionsV2;

typedef struct {
  double time_step;
  uint64_t substeps;
  bool enable_collisions;
  uint64_t collision_iterations;
  double gravitational_constant;
  uint64_t particle_count;
  Distribution position_x_distribution;
  Distribution position_y_distribution;
  Distribution mass_distribution;
  Distribution size_distribution;
  ParticleMode initial_particle_mode;
  VelocityInitMode velocity_init_mode;
  Distribution velocity_magnitude_distribution;
} SimulationOptions;

//
// Simulation Functions
//

Simulation init_simulation(SimulationOptions options);
void deinit_simulation(Simulation s);

void step_simulation(Simulation s);

SimulationOptions simulation_get_options(Simulation s);
void simulation_set_options(Simulation s, SimulationOptions options);

uint64_t simulation_get_particle_count(Simulation s);
SimulationParticle simulation_get_particle_state(Simulation s, uint64_t id);
void simulation_set_particle_state(Simulation s, uint64_t id,
                                   SimulationParticle particle);

uint64_t simulation_get_particle_count_in_circle(Simulation s, Vec2 center,
                                                 double radius);
uint64_t simulation_get_particle_count_in_fixed_rect(Simulation s,
                                                     Vec2 top_left,
                                                     Vec2 bottom_right);
uint64_t simulation_get_particle_count_in_tri(Simulation s, Vec2 p1, Vec2 p2,
                                              Vec2 p3);

Iterator simulation_get_particles_in_circle(Simulation s, ArenaAllocator *arena,
                                            Vec2 center, double radius);
Iterator simulation_get_particles_in_fixed_rect(Simulation s,
                                                ArenaAllocator *arena,
                                                Vec2 top_left,
                                                Vec2 bottom_right);
Iterator simulation_get_particles_in_tri(Simulation s, ArenaAllocator *arena,
                                         Vec2 p1, Vec2 p2, Vec2 p3);

Iterator simulation_get_particle_states_in_circle(Simulation s,
                                                  ArenaAllocator *arena,
                                                  Vec2 center, double radius);
Iterator simulation_get_particle_states_in_fixed_rect(Simulation s,
                                                      ArenaAllocator *arena,
                                                      Vec2 top_left,
                                                      Vec2 bottom_right);
Iterator simulation_get_particle_states_in_tri(Simulation s,
                                               ArenaAllocator *arena, Vec2 p1,
                                               Vec2 p2, Vec2 p3);

void simulation_set_particles_states_in_circle(Simulation s, Vec2 center,
                                               double radius,
                                               SimulationParticleDiff diff);
void simulation_set_particle_states_in_fixed_rect(Simulation s, Vec2 top_left,
                                                  Vec2 bottom_right,
                                                  SimulationParticleDiff diff);
void simulation_set_particle_states_in_tri(Simulation s, Vec2 p1, Vec2 p2,
                                           Vec2 p3,
                                           SimulationParticleDiff diff);

void simulation_new_particle(Simulation s, SimulationParticle particle);
void simulation_new_particles(Simulation s, SimulationParticle *particle,
                              size_t particle_count);
void simulation_new_particle_batch_in_circle(Simulation s,
                                             SimulationParticle particle,
                                             uint64_t particle_count);
void simulation_new_particle_batch_in_fixed_rect(Simulation s,
                                                 SimulationParticle particle,
                                                 uint64_t particle_count);
void simulation_new_particle_batch_in_tri(Simulation s,
                                          SimulationParticle particle,
                                          uint64_t particle_count);

//
// Simulation Helper Functions
//

SimulationParticle simulation_particle_to_mode(SimulationParticle p,
                                               ParticleMode to_mode);

/*int get_particles_in_rectangle(Simulation sim, Vec2 top_left,*/
/*                               Vec2 bottom_right, int *particle_ids,*/
/*                               int max_count);*/
/*int get_particles_in_circle(Simulation sim, Vec2 center, float radius,*/
/*                            int *particle_ids, int max_count);*/
/*void get_position_range(Simulation sim, Vec2 *min, Vec2 *max);*/
/*uint64_t get_particle_count(Simulation sim);*/
/*Particle get_particle_state(Simulation sim, int particle_id);*/
/*void set_particle_state(Simulation sim, int particle_id, Particle p);*/

#endif // GRAVITY_H
