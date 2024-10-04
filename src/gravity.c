#ifndef GRAVITY_C
#define GRAVITY_C

#include "gravity.h"
#include "arena_allocator.h"
#include "cglm/types-struct.h"
#include "iterator.h"
#include "logging.h"
#include "verlet.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include <cglm/struct.h>

#define GRAVITATIONAL_CONSTANT 6.67430e-11

double calculate_distance(vec2s *v1, vec2s *v2);
vec2s calculate_force(SimulationParticle *p1, SimulationParticle *p2,
                      double gravitational_constant);
void verlet_integration(Particle *p, vec2s total_force, double time_step);
#include <stddef.h>
#include <stdlib.h>

// Define the concrete Simulation struct
typedef struct {
  SimulationParticle *particles; // Array of particles
  uint64_t particle_count;       // Number of particles
  bool enable_collisions;        // Collision handling flag
  double time_step;              // Time step for the simulation
  uint64_t substeps;             // Number of substeps
  uint64_t
      collision_iterations; // Number of iterations for collision resolution
  vec2s *forces;            // Array to store forces for each particle
  double gravitational_constant; // Gravitational constant for the simulation

  SimulationOptions options;
} SimulationStruct;

double random_uniform_double(double min, double max) {
  return min + (double)rand() / RAND_MAX * (max - min);
}

double random_normal_double(double mean, double stddev) {
  double u1 = (double)rand() / RAND_MAX;
  double u2 = (double)rand() / RAND_MAX;
  double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
  return z0 * stddev + mean;
}

void set_particle_acceleration(SimulationStruct *s, uint64_t id);
void integrate_particle(SimulationStruct *s, uint64_t id);
void resolve_collision(SimulationStruct *s, uint64_t p1_id, uint64_t p2_id);

#include "gravity.h"

//
// Simulation Functions
//

Simulation init_simulation(SimulationOptions options) {
  SimulationStruct *s = (SimulationStruct *)malloc(sizeof(SimulationStruct));
  if (s == NULL) {
    return NULL;
  }

  s->particles = NULL;
  s->particle_count = 0;
  s->options = options;

  return (Simulation)s;
}

void deinit_simulation(Simulation s) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;
  free(simulation);
}

SimulationOptions simulation_get_options(Simulation s) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;
  return simulation->options;
}

void step_simulation(Simulation s) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;

  for (uint64_t substep = 0; substep < simulation->substeps; ++substep) {
    for (uint64_t id = 0; id < simulation->particle_count; ++id) {
      set_particle_acceleration(simulation, id);
    }
    for (uint64_t id = 0; id < simulation->particle_count; ++id) {
      integrate_particle(simulation, id);
    }
    for (uint64_t id_1 = 0; id_1 < simulation->particle_count; ++id_1) {
      for (uint64_t id_2 = 0; id_2 < simulation->particle_count; ++id_2) {
        resolve_collision(simulation, id_1, id_2);
      }
    }
  }
}

void simulation_set_options(Simulation s, SimulationOptions options) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;
  simulation->options = options;
}

uint64_t simulation_get_particle_count(Simulation s) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;
  return simulation->particle_count;
}

SimulationParticle simulation_get_particle_state(Simulation s, uint64_t id) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;
  assert(id < simulation->particle_count);
  return simulation->particles[id];
}

void simulation_set_particle_state(Simulation s, uint64_t id,
                                   SimulationParticle particle) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;
  assert(id < simulation->particle_count);
  simulation->particles[id] = particle;
}

uint64_t simulation_get_particle_count_in_circle(Simulation s, vec2s center,
                                                 double radius);
uint64_t simulation_get_particle_count_in_fixed_rect(Simulation s,
                                                     vec2s top_left,
                                                     vec2s bottom_right) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;

  uint64_t count = 0;
  for (size_t id = 0; id < simulation->particle_count; ++id) {
    SimulationParticle particle = simulation->particles[id];
    switch (particle.mode) {
    case PARTICLE_MODE_STATIC:
      // TODO: Change fixed rects to bottom_left and top_right. Flip the Y axis
      if (particle.params.STATIC.position.x > top_left.x &&
          particle.params.STATIC.position.x < bottom_right.x &&
          particle.params.STATIC.position.y > top_left.y &&
          particle.params.STATIC.position.y < bottom_right.y) {
        count += 1;
      }
      break;
    case PARTICLE_MODE_VERLET:
      if (particle.params.VERLET.position.x > top_left.x &&
          particle.params.VERLET.position.x < bottom_right.x &&
          particle.params.VERLET.position.y > top_left.y &&
          particle.params.VERLET.position.y < bottom_right.y) {
        count += 1;
      }
      break;
    }
  }
  return count;
}

uint64_t simulation_get_particle_count_in_tri(Simulation s, vec2s p1, vec2s p2,
                                              vec2s p3);

Iterator simulation_get_particles_in_circle(Simulation s, ArenaAllocator *arena,
                                            vec2s center, double radius) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;

  uint64_t count = 0;
  uint64_t *particles = NULL;

  for (size_t id = 0; id < simulation->particle_count; ++id) {
    SimulationParticle particle = simulation->particles[id];
    vec2s position;
    switch (particle.mode) {
    case PARTICLE_MODE_STATIC:
      position = particle.params.STATIC.position;
      break;
    case PARTICLE_MODE_VERLET:
      position = particle.params.VERLET.position;
      break;
    }

    double dx = position.x - center.x;
    double dy = position.y - center.y;
    double distance_squared = dx * dx + dy * dy;
    if (distance_squared <= radius * radius) {
      uint64_t *p = arena_alloc(arena, sizeof(uint64_t)); // Append
      assert(p);
      if (particles == NULL) {
        particles = p;
      }
      *p = id;
      count += 1;
    }
  }

  return create_iterator(particles, count, sizeof(uint64_t));
}

Iterator simulation_get_particles_in_fixed_rect(Simulation s,
                                                ArenaAllocator *arena,
                                                vec2s top_left,
                                                vec2s bottom_right) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;

  uint64_t count = 0;
  uint64_t *particles = NULL;

  for (size_t id = 0; id < simulation->particle_count; ++id) {
    SimulationParticle particle = simulation->particles[id];
    switch (particle.mode) {
    case PARTICLE_MODE_STATIC:
      // TODO: Change fixed rects to bottom_left and top_right. Flip the Y axis
      if (particle.params.STATIC.position.x > top_left.x &&
          particle.params.STATIC.position.x < bottom_right.x &&
          particle.params.STATIC.position.y > top_left.y &&
          particle.params.STATIC.position.y < bottom_right.y) {
        uint64_t *p = arena_alloc(arena, sizeof(uint64_t)); // Append
        assert(p);
        if (particles == NULL) {
          particles = p;
        }
        *p = id;
        count += 1;
      }
      break;
    case PARTICLE_MODE_VERLET:
      if (particle.params.VERLET.position.x > top_left.x &&
          particle.params.VERLET.position.x < bottom_right.x &&
          particle.params.VERLET.position.y > top_left.y &&
          particle.params.VERLET.position.y < bottom_right.y) {
        uint64_t *p = arena_alloc(arena, sizeof(uint64_t)); // Append
        assert(p);
        if (particles == NULL) {
          particles = p;
        }
        *p = id;
        count += 1;
      }
      break;
    }
  }

  return create_iterator(particles, count, sizeof(uint64_t));
}

Iterator simulation_get_particles_in_tri(Simulation s, ArenaAllocator *arena,
                                         vec2s p1, vec2s p2, vec2s p3);

Iterator simulation_get_particle_states_in_circle(Simulation s,
                                                  ArenaAllocator *arena,
                                                  vec2s center, double radius);
Iterator simulation_get_particle_states_in_fixed_rect(Simulation s,
                                                      ArenaAllocator *arena,
                                                      vec2s top_left,
                                                      vec2s bottom_right) {
  assert(s);
  SimulationStruct *simulation = (SimulationStruct *)s;

  uint64_t count = 0;
  SimulationParticle *particles = NULL;

  for (size_t id = 0; id < simulation->particle_count; ++id) {
    SimulationParticle particle = simulation->particles[id];
    switch (particle.mode) {
    case PARTICLE_MODE_STATIC:
      // TODO: Change fixed rects to bottom_left and top_right. Flip the Y axis
      if (particle.params.STATIC.position.x > top_left.x &&
          particle.params.STATIC.position.x < bottom_right.x &&
          particle.params.STATIC.position.y > top_left.y &&
          particle.params.STATIC.position.y < bottom_right.y) {
        SimulationParticle *p =
            arena_alloc(arena, sizeof(SimulationParticle)); // Append
        assert(p);
        if (particles == NULL) {
          particles = p;
        }
        *p = simulation->particles[id];
        count += 1;
      }
      break;
    case PARTICLE_MODE_VERLET:
      if (particle.params.VERLET.position.x > top_left.x &&
          particle.params.VERLET.position.x < bottom_right.x &&
          particle.params.VERLET.position.y > top_left.y &&
          particle.params.VERLET.position.y < bottom_right.y) {
        SimulationParticle *p =
            arena_alloc(arena, sizeof(SimulationParticle)); // Append
        assert(p);
        if (particles == NULL) {
          particles = p;
        }
        *p = simulation->particles[id];
        count += 1;
      }
      break;
    }
  }

  return create_iterator(particles, count, sizeof(SimulationParticle));
}
Iterator simulation_get_particle_states_in_tri(Simulation s,
                                               ArenaAllocator *arena, vec2s p1,
                                               vec2s p2, vec2s p3);

void simulation_set_particles_states_in_circle(Simulation s, vec2s center,
                                               double radius,
                                               SimulationParticleDiff diff);
void simulation_set_particle_states_in_fixed_rect(Simulation s, vec2s top_left,
                                                  vec2s bottom_right,
                                                  SimulationParticleDiff diff);
void simulation_set_particle_states_in_tri(Simulation s, vec2s p1, vec2s p2,
                                           vec2s p3,
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
                                               ParticleMode to_mode) {
  SimulationParticle result = (SimulationParticle){0};
  switch (p.mode) {
  case PARTICLE_MODE_STATIC:
    switch (to_mode) {
    case PARTICLE_MODE_STATIC:
      result = p;
      break;
    case PARTICLE_MODE_VERLET:
      result.mode = PARTICLE_MODE_VERLET;
      result.params.VERLET.position = p.params.STATIC.position;
      result.params.VERLET.position_previous = p.params.STATIC.position;
      result.params.VERLET.acceleration = (vec2s){0};
      result.params.VERLET.mass = p.params.STATIC.mass;
      result.params.VERLET.radius = p.params.STATIC.radius;
      break;
    }
    break;
  case PARTICLE_MODE_VERLET:
    switch (to_mode) {
    case PARTICLE_MODE_STATIC:
      result.mode = PARTICLE_MODE_STATIC;
      result.params.STATIC.position = p.params.VERLET.position;
      result.params.STATIC.mass = p.params.VERLET.mass;
      result.params.STATIC.radius = p.params.VERLET.radius;
      break;
    case PARTICLE_MODE_VERLET:
      result = p;
      break;
    }
    break;
  }
  return result;
}

double sample_distribution(const Distribution *dist) {
  switch (dist->type) {
  case DISTRIBUTION_UNIFORM:
    return random_uniform_double(dist->params.uniform.min,
                                 dist->params.uniform.max);
  case DISTRIBUTION_NORMAL:
    return random_normal_double(dist->params.normal.mean,
                                dist->params.normal.stddev);
  // Add cases for other distribution types as needed
  default:
    fprintf(stderr, "Unknown distribution type\n");
    return 0.0;
  }
}

//
// Internal Helper Functions
//

void set_particle_acceleration(SimulationStruct *s, uint64_t id) {
  assert(id < s->particle_count);
  if (s->particles[id].mode == PARTICLE_MODE_STATIC) {
    return;
  }

  s->particles[id].params.VERLET.acceleration = (vec2s){0};
  for (uint64_t i = 0; i < s->particle_count; ++i) {
    if (i == id)
      continue; // Skip self-interaction

    SimulationParticle *other = &s->particles[i];
    vec2s force =
        calculate_force(&s->particles[id], other, s->gravitational_constant);
    s->particles[id].params.VERLET.acceleration.x += force.x;
    s->particles[id].params.VERLET.acceleration.y += force.y;
  }
  // TODO: Loop through particles and accumulate acceleration
}

void integrate_particle(SimulationStruct *s, uint64_t id) {
  assert(id < s->particle_count);
  // TODO: Verlet step the particle
}

void resolve_collision(SimulationStruct *s, uint64_t id_1, uint64_t id_2) {
  assert(id_1 < s->particle_count);
  assert(id_2 < s->particle_count);

  // TODO: Resolve collision between the two particles
}

/*Simulation init_simulation(SimulationOptions options) {*/
/*  // Allocate memory for the simulation*/
/*  SimulationStruct *sim = (SimulationStruct
 * *)malloc(sizeof(SimulationStruct));*/
/*  if (!sim) {*/
/*    return NULL; // Allocation failed*/
/*  }*/
/**/
/*  // Initialize the simulation with the given options*/
/*  sim->particle_count = options.particle_count;*/
/*  sim->enable_collisions = options.enable_collisions;*/
/*  sim->time_step = options.time_step;*/
/*  sim->substeps = options.substeps;*/
/**/
/*  // Store gravitational constant*/
/*  sim->gravitational_constant = options.gravitational_constant;*/
/**/
/*  // Store collision iterations*/
/*  sim->collision_iterations = options.collision_iterations;*/
/**/
/*  // Allocate memory for particles*/
/*  sim->particles = (Particle *)malloc(sizeof(Particle) *
 * sim->particle_count);*/
/*  if (!sim->particles) {*/
/*    free(sim);*/
/*    return NULL; // Allocation failed*/
/*  }*/
/**/
/*  // Initialize particles*/
/*  for (uint64_t i = 0; i < sim->particle_count; i++) {*/
/*    sim->particles[i].position.x =*/
/*        sample_distribution(&options.position_x_distribution);*/
/*    sim->particles[i].position.y =*/
/*        sample_distribution(&options.position_y_distribution);*/
/*    sim->particles[i].mass =
 * sample_distribution(&options.mass_distribution);*/
/*    sim->particles[i].size =
 * sample_distribution(&options.size_distribution);*/
/**/
/*    sim->particles[i].mode = options.initial_particle_mode;*/
/*    double velocity_magnitude =*/
/*        sample_distribution(&options.velocity_magnitude_distribution);*/
/**/
/*    switch (options.velocity_init_mode) {*/
/*    case VELOCITY_INIT_ZERO:*/
/*      sim->particles[i].velocity.x = 0;*/
/*      sim->particles[i].velocity.y = 0;*/
/*      break;*/
/**/
/*    case VELOCITY_INIT_RANDOM: {*/
/*      double angle = random_uniform_double(0, 2 * M_PI);*/
/*      sim->particles[i].velocity.x = velocity_magnitude * cos(angle);*/
/*      sim->particles[i].velocity.y = velocity_magnitude * sin(angle);*/
/*    } break;*/
/**/
/*    case VELOCITY_INIT_PERPENDICULAR_TO_ORIGIN: {*/
/*      double dx = sim->particles[i].position.x;*/
/*      double dy = sim->particles[i].position.y;*/
/*      double distance = sqrt(dx * dx + dy * dy);*/
/*      sim->particles[i].velocity.x = velocity_magnitude * (-dy / distance);*/
/*      sim->particles[i].velocity.y = velocity_magnitude * (dx / distance);*/
/*    } break;*/
/**/
/*    case VELOCITY_INIT_TOWARDS_ORIGIN: {*/
/*      double dx = -sim->particles[i].position.x;*/
/*      double dy = -sim->particles[i].position.y;*/
/*      double distance = sqrt(dx * dx + dy * dy);*/
/*      sim->particles[i].velocity.x = velocity_magnitude * (dx / distance);*/
/*      sim->particles[i].velocity.y = velocity_magnitude * (dy / distance);*/
/*      sim->particles[i].mode = PARTICLE_MODE_VERLET;*/
/*    } break;*/
/**/
/*    case VELOCITY_INIT_AWAY_FROM_ORIGIN: {*/
/*      double dx = sim->particles[i].position.x;*/
/*      double dy = sim->particles[i].position.y;*/
/*      double distance = sqrt(dx * dx + dy * dy);*/
/*      sim->particles[i].velocity.x = velocity_magnitude * (dx / distance);*/
/*      sim->particles[i].velocity.y = velocity_magnitude * (dy / distance);*/
/*    } break;*/
/**/
/*    default:*/
/*      fprintf(stderr, "Unknown velocity initialization mode\n");*/
/*      sim->particles[i].velocity.x = 0;*/
/*      sim->particles[i].velocity.y = 0;*/
/*    }*/
/*  }*/
/**/
/*  // Allocate memory for forces*/
/*  sim->forces = (vec2s *)malloc(sizeof(vec2s) * sim->particle_count);*/
/*  if (!sim->forces) {*/
/*    free(sim->particles);*/
/*    free(sim);*/
/*    return NULL; // Allocation failed*/
/*  }*/
/**/
/*  return (Simulation)sim;*/
/*}*/

/*void deinit_simulation(Simulation sim) {*/
/*  SimulationStruct *sim_struct = (SimulationStruct *)sim;*/
/*  if (sim_struct) {*/
/*    // Free the particles array*/
/*    free(sim_struct->particles);*/
/*    // Free the forces array*/
/*    free(sim_struct->forces);*/
/*    free(sim_struct);*/
/*  }*/
/*}*/

/*void resolve_collision(Particle *p1, Particle *p2) {*/
/*  double dx = p2->position.x - p1->position.x;*/
/*  double dy = p2->position.y - p1->position.y;*/
/*  double distance = sqrt(dx * dx + dy * dy);*/
/*  double min_distance = p1->size + p2->size;*/
/**/
/*  if (distance < min_distance) {*/
/*    // Calculate overlap*/
/*    double overlap = min_distance - distance;*/
/**/
/*    // Calculate the normal vector*/
/*    double nx = dx / distance;*/
/*    double ny = dy / distance;*/
/**/
/*    // Determine how to resolve the collision based on particle modes*/
/*    if (p1->mode == PARTICLE_MODE_VERLET && p2->mode == PARTICLE_MODE_VERLET)
 * {*/
/*      // Both particles are dynamic, move each half the overlap*/
/*      p1->position.x -= 0.5 * overlap * nx;*/
/*      p1->position.y -= 0.5 * overlap * ny;*/
/*      p2->position.x += 0.5 * overlap * nx;*/
/*      p2->position.y += 0.5 * overlap * ny;*/
/*    } else if (p1->mode == PARTICLE_MODE_VERLET &&*/
/*               p2->mode == PARTICLE_MODE_STATIC) {*/
/*      // p1 is dynamic, p2 is static, move p1 full overlap*/
/*      p1->position.x -= overlap * nx;*/
/*      p1->position.y -= overlap * ny;*/
/*    } else if (p1->mode == PARTICLE_MODE_STATIC &&*/
/*               p2->mode == PARTICLE_MODE_VERLET) {*/
/*      // p1 is static, p2 is dynamic, move p2 full overlap*/
/*      p2->position.x += overlap * nx;*/
/*      p2->position.y += overlap * ny;*/
/*    }*/
/*    // If both are static, do nothing*/
/**/
/*    // Calculate the relative velocity*/
/*    double dvx = p2->velocity.x - p1->velocity.x;*/
/*    double dvy = p2->velocity.y - p1->velocity.y;*/
/**/
/*    // Calculate the velocity along the normal*/
/*    double vn = dvx * nx + dvy * ny;*/
/**/
/*    // If the particles are moving apart, no need to resolve*/
/*    if (vn > 0)*/
/*      return;*/
/**/
/*    // Introduce a damping factor to the impulse*/
/*    double damping_factor = 0.999; // Adjust this value as needed*/
/*    double impulse = damping_factor * (2 * vn) / (p1->mass + p2->mass);*/
/**/
/*    // Update velocities based on the impulse*/
/*    p1->velocity.x += impulse * p2->mass * nx;*/
/*    p1->velocity.y += impulse * p2->mass * ny;*/
/*    p2->velocity.x -= impulse * p1->mass * nx;*/
/*    p2->velocity.y -= impulse * p1->mass * ny;*/
/*  }*/
/*}*/

/*void calculate_forces(SimulationStruct *sim_struct) {*/
/*  for (uint64_t i = 0; i < sim_struct->particle_count; ++i) {*/
/*    Particle *p = &sim_struct->particles[i];*/
/**/
/*    // Skip force calculation for static particles*/
/*    if (p->mode == PARTICLE_MODE_STATIC) {*/
/*      sim_struct->forces[i] = (vec2s){{0.0, 0.0}};*/
/*      continue;*/
/*    }*/
/**/
/*    // Reset force accumulator*/
/*    sim_struct->forces[i] = (vec2s){{0.0, 0.0}};*/
/**/
/*    for (uint64_t j = 0; j < sim_struct->particle_count; ++j) {*/
/*      if (i == j)*/
/*        continue; // Skip self-interaction*/
/**/
/*      Particle *other = &sim_struct->particles[j];*/
/*      vec2s force =*/
/*          calculate_force(p, other, sim_struct->gravitational_constant);*/
/*      sim_struct->forces[i].x += force.x;*/
/*      sim_struct->forces[i].y += force.y;*/
/*    }*/
/*  }*/
/*}*/

/*void integrate_particles(SimulationStruct *sim_struct, double substep_time)
 * {*/
/*  for (uint64_t i = 0; i < sim_struct->particle_count; ++i) {*/
/*    Particle *p = &sim_struct->particles[i];*/
/**/
/*    switch (p->mode) {*/
/*    case PARTICLE_MODE_STATIC: {*/
/*      continue;*/
/*    } break;*/
/*case PARTICLE_MODE_DYNAMIC: {*/
/*  verlet_integration(p, sim_struct->forces[i], substep_time);*/
/*} break;*/
/*    case PARTICLE_MODE_VERLET: {*/
/*      VerletParticle vp =*/
/*          init_verlet_particle(p->position, p->velocity, substep_time);*/
/*      vp.acceleration = sim_struct->forces[i];*/
/*      verlet_step(&vp, substep_time);*/
/*      p->position = vp.position;*/
/*      p->velocity = compute_velocity(vp, substep_time);*/
/*    } break;*/
/*    }*/
/*  }*/
/*}*/

/*void step_simulation(Simulation sim) {*/
/*  SimulationStruct *sim_struct = (SimulationStruct *)sim;*/
/**/
/*  // Loop over each substep*/
/*  for (uint64_t substep = 0; substep < sim_struct->substeps; ++substep) {*/
/*    calculate_forces(sim_struct);*/
/*    double substep_time = sim_struct->time_step / sim_struct->substeps;*/
/*    integrate_particle(sim_struct, substep_time);*/
/*  }*/
/**/
/*  if (sim_struct->enable_collisions) {*/
/*    for (uint64_t iter = 0; iter < sim_struct->collision_iterations; ++iter)
 * {*/
/*      // Loop over each particle pair to check for collisions*/
/*      for (uint64_t i = 0; i < sim_struct->particle_count; ++i) {*/
/*        for (uint64_t j = i + 1; j < sim_struct->particle_count; ++j) {*/
/*          if (i == j)*/
/*            continue; // Skip self-interaction*/
/**/
/*          Particle *p1 = &sim_struct->particles[i];*/
/*          Particle *p2 = &sim_struct->particles[j];*/
/**/
/*          resolve_collision(p1, p2);*/
/*        }*/
/*      }*/
/*    }*/
/*  }*/
/*}*/

double calculate_distance(vec2s *v1, vec2s *v2) {
  double dx = v2->x - v1->x;
  double dy = v2->y - v1->y;
  return sqrt(dx * dx + dy * dy);
}

vec2s calculate_force(SimulationParticle *p1, SimulationParticle *p2,
                      double gravitational_constant) {
  vec2s p1_pos;
  vec2s p2_pos;
  double p1_size;
  double p2_size;
  double p1_mass;
  double p2_mass;
  switch (p1->mode) {
  case PARTICLE_MODE_STATIC:
    p1_pos = p1->params.STATIC.position;
    p1_size = p1->params.STATIC.radius;
    p1_mass = p1->params.STATIC.mass;
    break;
  case PARTICLE_MODE_VERLET:
    p1_pos = p1->params.VERLET.position;
    p1_size = p1->params.VERLET.radius;
    p1_mass = p1->params.VERLET.mass;
    break;
  }
  switch (p2->mode) {
  case PARTICLE_MODE_STATIC:
    p2_pos = p2->params.STATIC.position;
    p2_size = p2->params.STATIC.radius;
    p2_mass = p2->params.STATIC.mass;
    break;
  case PARTICLE_MODE_VERLET:
    p2_pos = p2->params.VERLET.position;
    p2_size = p2->params.VERLET.radius;
    p2_mass = p2->params.VERLET.mass;
    break;
  }

  vec2s force = {{0.0, 0.0}};
  vec2s distance_vector = {
      {p2_pos.x - p1_pos.x, p2_pos.y - p1_pos.y}};
  double distance = calculate_distance(&p1_pos, &p2_pos);
  double distance_squared = distance * distance;

  // TODO(lily): Scale the force down to zero as the overlap of the particles
  // increases instead of disabling the force
  double min_distance = p1_size + p2_size;
  if (distance > min_distance) {
    double force_magnitude =
        gravitational_constant * (p1_mass * p2_mass) / distance_squared;
    force.x = force_magnitude * (distance_vector.x / distance);
    force.y = force_magnitude * (distance_vector.y / distance);
  }

  return force;
}

/*void verlet_integration(Particle *p, vec2s total_force, double time_step) {*/
/*  vec2s acceleration = {{total_force.x / p->mass, total_force.y / p->mass}};*/
/**/
/*  // Update position*/
/*  p->position.x +=*/
/*      p->velocity.x * time_step + 0.5 * acceleration.x * time_step *
 * time_step;*/
/*  p->position.y +=*/
/*      p->velocity.y * time_step + 0.5 * acceleration.y * time_step *
 * time_step;*/
/**/
/*  // Update velocity*/
/*  p->velocity.x += acceleration.x * time_step;*/
/*  p->velocity.y += acceleration.y * time_step;*/
/*}*/

/*void toggle_collisions(bool enable) {*/
/*  (void)enable;*/
/*  // Enable or disable collision handling*/
/*}*/

/*Particle get_particle_state(Simulation sim, int particle_id) {*/
/*  SimulationStruct *sim_struct = (SimulationStruct *)sim;*/
/*  if (particle_id < 0 || (uint64_t)particle_id >= sim_struct->particle_count)
 * {*/
/*    // Handle invalid particle_id*/
/*    Particle empty_particle = {0};*/
/*    return empty_particle;*/
/*  }*/
/*  return sim_struct->particles[particle_id];*/
/*}*/
/**/
/*void set_particle_state(Simulation sim, int particle_id, Particle p) {*/
/*  SimulationStruct *sim_struct = (SimulationStruct *)sim;*/
/*  if (particle_id >= 0 && (uint64_t)particle_id < sim_struct->particle_count)
 * {*/
/*    sim_struct->particles[particle_id] = p;*/
/*  }*/
/*}*/
/**/
/*int get_particles_in_rectangle(Simulation sim, vec2s top_left,*/
/*                               vec2s bottom_right, int *particle_ids,*/
/*                               int max_count) {*/
/*  SimulationStruct *sim_struct = (SimulationStruct *)sim;*/
/*  int count = 0;*/
/**/
/*  if (!sim_struct || !particle_ids || max_count <= 0) {*/
/*    ERROR_LOG("Invalid parameters in get_particles_in_rectangle: "*/
/*              "sim_struct=%p, particle_ids=%p, max_count=%d",*/
/*              (void *)sim_struct, (void *)particle_ids, max_count);*/
/*    return -1;*/
/*  }*/
/**/
/*  double min_x = fmin(top_left.x, bottom_right.x);*/
/*  double max_x = fmax(top_left.x, bottom_right.x);*/
/*  double min_y = fmin(top_left.y, bottom_right.y);*/
/*  double max_y = fmax(top_left.y, bottom_right.y);*/
/**/
/*  DEBUG_LOG(*/
/*      "Starting particle search in rectangle (%.2f, %.2f) to (%.2f, %.2f)",*/
/*      min_x, min_y, max_x, max_y);*/
/*  TRACE_LOG("Total particle count: %llu", sim_struct->particle_count);*/
/**/
/*  for (uint64_t i = 0; i < sim_struct->particle_count && count < max_count;*/
/*       i++) {*/
/*    Particle *p = &sim_struct->particles[i];*/
/*    TRACE_LOG("Checking particle %llu at (%.2f, %.2f)", i, p->position.x,*/
/*              p->position.y);*/
/*    if (p->position.x >= min_x && p->position.x <= max_x &&*/
/*        p->position.y >= min_y && p->position.y <= max_y) {*/
/*      particle_ids[count++] = i;*/
/*      DEBUG_LOG("Particle %llu at (%.2f, %.2f) is inside the rectangle", i,*/
/*                p->position.x, p->position.y);*/
/*    }*/
/*  }*/
/**/
/*  DEBUG_LOG("Finished particle search. Found %d particles in rectangle (%.2f,
 * "*/
/*            "%.2f) to (%.2f, %.2f)",*/
/*            count, min_x, min_y, max_x, max_y);*/
/**/
/*  return count;*/
/*}*/
/**/
/*int get_particles_in_circle(Simulation sim, vec2s center, float radius,*/
/*                            int *particle_ids, int max_count) {*/
/*  SimulationStruct *sim_struct = (SimulationStruct *)sim;*/
/*  int count = 0;*/
/*  float radius_squared = radius * radius;*/
/**/
/*  for (uint64_t i = 0; i < sim_struct->particle_count && count < max_count;*/
/*       i++) {*/
/*    Particle *p = &sim_struct->particles[i];*/
/*    float dx = p->position.x - center.x;*/
/*    float dy = p->position.y - center.y;*/
/*    if (dx * dx + dy * dy <= radius_squared) {*/
/*      particle_ids[count++] = i;*/
/*    }*/
/*  }*/
/**/
/*  return count;*/
/*}*/

/*// Adds a new particle to the simulation.*/
/*void add_particle(double mass, double size, vec2s position, vec2s velocity)
 * {*/
/*  (void)mass;*/
/*  (void)size;*/
/*  (void)position;*/
/*  (void)velocity;*/
/*  // Add a new particle with specified properties*/
/*}*/
/**/
/*// Removes a particle from the simulation.*/
/*void remove_particle(int particle_id) {*/
/*  (void)particle_id;*/
/*  // Remove the particle with the given ID*/
/*}*/
/**/
/*// Applies an external force to a specific particle.*/
/*void apply_force(int particle_id, vec2s force) {*/
/*  (void)particle_id;*/
/*  (void)force;*/
/*  // Apply the specified force to the particle*/
/*}*/
/*void get_position_range(Simulation sim, vec2s *min, vec2s *max) {*/
/*  SimulationStruct *sim_struct = (SimulationStruct *)sim;*/
/**/
/*  // Initialize min and max with the first particle's position*/
/*  *min = sim_struct->particles[0].position;*/
/*  *max = sim_struct->particles[0].position;*/
/**/
/*  // Iterate through all particles to find the actual min and max*/
/*  for (uint64_t i = 1; i < sim_struct->particle_count; i++) {*/
/*    vec2s pos = sim_struct->particles[i].position;*/
/**/
/*    if (pos.x < min->x)*/
/*      min->x = pos.x;*/
/*    if (pos.y < min->y)*/
/*      min->y = pos.y;*/
/*    if (pos.x > max->x)*/
/*      max->x = pos.x;*/
/*    if (pos.y > max->y)*/
/*      max->y = pos.y;*/
/*  }*/
/*}*/
/**/
/*uint64_t get_particle_count(Simulation sim) {*/
/*  SimulationStruct *sim_struct = (SimulationStruct *)sim;*/
/*  return sim_struct->particle_count;*/
/*}*/

#endif // GRAVITY_C
