#ifndef GRAVITY_C
#define GRAVITY_C

#include "gravity.h"

double sample_distribution(const Distribution *dist);
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

  // Temporary code to spawn initial particles
  // This code is due to be removed; initial particle conditions will happen
  // outside of the init function
  s->particles = (SimulationParticle *)malloc(sizeof(SimulationParticle) *
                                              options.particle_count);
  if (!s->particles) {
    free(s);
    return NULL; // Allocation failed
  }
  s->particle_count = options.particle_count;

  for (uint64_t i = 0; i < s->particle_count; i++) {
    s->particles[i].mode = options.initial_particle_mode;
    switch (options.initial_particle_mode) {
    case PARTICLE_MODE_STATIC:
      s->particles[i].params.STATIC.position.x =
          sample_distribution(&options.position_x_distribution);
      s->particles[i].params.STATIC.position.y =
          sample_distribution(&options.position_y_distribution);
      s->particles[i].params.STATIC.mass =
          sample_distribution(&options.mass_distribution);
      s->particles[i].params.STATIC.radius =
          sample_distribution(&options.size_distribution);
      break;
    case PARTICLE_MODE_VERLET:
      s->particles[i].params.VERLET.position.x =
          sample_distribution(&options.position_x_distribution);
      s->particles[i].params.VERLET.position.y =
          sample_distribution(&options.position_y_distribution);
      s->particles[i].params.VERLET.position_previous =
          s->particles[i].params.VERLET.position;
      s->particles[i].params.VERLET.acceleration = (vec2s){0};
      s->particles[i].params.VERLET.mass =
          sample_distribution(&options.mass_distribution);
      s->particles[i].params.VERLET.radius =
          sample_distribution(&options.size_distribution);
      break;
    }
  }

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
  if (id >= simulation->particle_count) {
    fprintf(stderr, "Error: Attempted to access particle with id %lu, but there are only %lu particles.\n", id, simulation->particle_count);
    // Return a default particle or handle the error in a way that doesn't crash the program
    return (SimulationParticle){0};
  }
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

  // Ensure top_left.x is actually the leftmost x-coordinate
  double left_x = fmin(top_left.x, bottom_right.x);
  double right_x = fmax(top_left.x, bottom_right.x);
  // Ensure top_left.y is actually the topmost y-coordinate
  double top_y = fmax(top_left.y, bottom_right.y);
  double bottom_y = fmin(top_left.y, bottom_right.y);

  DEBUG_LOG("Searching for particles in rectangle: (%f, %f) to (%f, %f)", left_x, bottom_y, right_x, top_y);
  DEBUG_LOG("Total particle count: %lu", simulation->particle_count);

  for (uint64_t id = 0; id < simulation->particle_count; ++id) {
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

    if (position.x >= left_x && position.x <= right_x &&
        position.y >= bottom_y && position.y <= top_y) {
      uint64_t *p = arena_alloc(arena, sizeof(uint64_t)); // Append
      assert(p);
      if (particles == NULL) {
        particles = p;
      }
      *p = id;
      count += 1;
    }
  }

  DEBUG_LOG("Found %lu particles in rectangle", count);

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
}

void integrate_particle(SimulationStruct *s, uint64_t id) {
  assert(id < s->particle_count);

  switch (s->particles[id].mode) {
  case PARTICLE_MODE_STATIC: {
    return;
  } break;
  case PARTICLE_MODE_VERLET: {
    vec2s new_position = verlet_step(
        s->particles[id].params.VERLET.position,
        s->particles[id].params.VERLET.position_previous,
        s->particles[id].params.VERLET.acceleration, s->options.time_step);
    s->particles[id].params.VERLET.position_previous =
        s->particles[id].params.VERLET.position;
    s->particles[id].params.VERLET.position = new_position;
  } break;
  }
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

double calculate_distance(vec2s *v1, vec2s *v2) {
  double dx = v2->x - v1->x;
  double dy = v2->y - v1->y;
  return sqrt(dx * dx + dy * dy);
}

vec2s calculate_force(SimulationParticle *p1, SimulationParticle *p2,
                      double gravitational_constant) {
  vec2s p1_pos;
  vec2s p2_pos;
  double p1_mass;
  double p2_mass;
  switch (p1->mode) {
  case PARTICLE_MODE_STATIC:
    p1_pos = p1->params.STATIC.position;
    p1_mass = p1->params.STATIC.mass;
    break;
  case PARTICLE_MODE_VERLET:
    p1_pos = p1->params.VERLET.position;
    p1_mass = p1->params.VERLET.mass;
    break;
  }
  switch (p2->mode) {
  case PARTICLE_MODE_STATIC:
    p2_pos = p2->params.STATIC.position;
    p2_mass = p2->params.STATIC.mass;
    break;
  case PARTICLE_MODE_VERLET:
    p2_pos = p2->params.VERLET.position;
    p2_mass = p2->params.VERLET.mass;
    break;
  }

  vec2s force = {{0.0, 0.0}};
  vec2s distance_vector = {{p2_pos.x - p1_pos.x, p2_pos.y - p1_pos.y}};
  double distance = calculate_distance(&p1_pos, &p2_pos);
  double distance_squared = distance * distance;

  double force_magnitude =
      gravitational_constant * (p1_mass * p2_mass) / distance_squared;
  force.x = force_magnitude * (distance_vector.x / distance);
  force.y = force_magnitude * (distance_vector.y / distance);

  return force;
}

#endif // GRAVITY_C
