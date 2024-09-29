#ifndef GRAVITY_C
#define GRAVITY_C

#include "gravity.h"
#include <math.h>

#define GRAVITATIONAL_CONSTANT 6.67430e-7

double calculate_distance(Vector2D *v1, Vector2D *v2);
Vector2D calculate_force(Particle *p1, Particle *p2,
                         double gravitational_constant);
void verlet_integration(Particle *p, Vector2D total_force, double time_step);
#include <stddef.h>
#include <stdlib.h>

// Define the concrete Simulation struct
typedef struct {
  Particle *particles;        // Array of particles
  uint64_t particle_count;    // Number of particles
  bool enable_collisions;     // Collision handling flag
  double time_step;           // Time step for the simulation
  uint64_t substeps;          // Number of substeps
  Vector2D position_range[2]; // Position range
  uint64_t
      collision_iterations; // Number of iterations for collision resolution
  Vector2D *forces;         // Array to store forces for each particle
  double gravitational_constant; // Gravitational constant for the simulation
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

double sample_distribution(const Distribution* dist) {
    switch (dist->type) {
        case DISTRIBUTION_UNIFORM:
            return random_uniform_double(dist->params.uniform.min, dist->params.uniform.max);
        case DISTRIBUTION_NORMAL:
            return random_normal_double(dist->params.normal.mean, dist->params.normal.stddev);
        // Add cases for other distribution types as needed
        default:
            fprintf(stderr, "Unknown distribution type\n");
            return 0.0;
    }
}

Simulation init_simulation(SimulationOptions options) {
  // Allocate memory for the simulation
  SimulationStruct *sim = (SimulationStruct *)malloc(sizeof(SimulationStruct));
  if (!sim) {
    return NULL; // Allocation failed
  }

  // Initialize the simulation with the given options
  sim->particle_count = options.particle_count;
  sim->enable_collisions = options.enable_collisions;
  sim->time_step = options.time_step;
  sim->substeps = options.substeps;

  // Store gravitational constant
  sim->gravitational_constant = options.gravitational_constant;
  sim->position_range[0] = options.position_range[0];
  sim->position_range[1] = options.position_range[1];

  // Store collision iterations
  sim->collision_iterations = options.collision_iterations;

  // Allocate memory for particles
  sim->particles = (Particle *)malloc(sizeof(Particle) * sim->particle_count);
  if (!sim->particles) {
    free(sim);
    return NULL; // Allocation failed
  }

  // Initialize particles
  for (uint64_t i = 0; i < sim->particle_count; i++) {
    sim->particles[i].position.x = sample_distribution(&options.position_x_distribution);
    sim->particles[i].position.y = sample_distribution(&options.position_y_distribution);
    sim->particles[i].mass = sample_distribution(&options.mass_distribution);
    sim->particles[i].size = sample_distribution(&options.size_distribution);
    sim->particles[i].velocity.x = sample_distribution(&options.velocity_x_distribution);
    sim->particles[i].velocity.y = sample_distribution(&options.velocity_y_distribution);
  }

  // Allocate memory for forces
  sim->forces = (Vector2D *)malloc(sizeof(Vector2D) * sim->particle_count);
  if (!sim->forces) {
    free(sim->particles);
    free(sim);
    return NULL; // Allocation failed
  }

  return (Simulation)sim;
}

void deinit_simulation(Simulation sim) {
  SimulationStruct *sim_struct = (SimulationStruct *)sim;
  if (sim_struct) {
    // Free the particles array
    free(sim_struct->particles);
    // Free the forces array
    free(sim_struct->forces);
    free(sim_struct);
  }
}

void resolve_collision(Particle *p1, Particle *p2) {
  double dx = p2->position.x - p1->position.x;
  double dy = p2->position.y - p1->position.y;
  double distance = sqrt(dx * dx + dy * dy);
  double min_distance = p1->size + p2->size;

  if (distance < min_distance) {
    // Calculate overlap
    double overlap = 0.5 * (min_distance - distance);

    // Displace particles to resolve overlap
    p1->position.x -= overlap * (dx / distance);
    p1->position.y -= overlap * (dy / distance);
    p2->position.x += overlap * (dx / distance);
    p2->position.y += overlap * (dy / distance);

    // Calculate the normal vector
    double nx = dx / distance;
    double ny = dy / distance;

    // Calculate the relative velocity
    double dvx = p2->velocity.x - p1->velocity.x;
    double dvy = p2->velocity.y - p1->velocity.y;

    // Calculate the velocity along the normal
    double vn = dvx * nx + dvy * ny;

    // If the particles are moving apart, no need to resolve
    if (vn > 0)
      return;

    // Introduce a damping factor to the impulse
    double damping_factor = 0.999; // Adjust this value as needed
    double impulse = damping_factor * (2 * vn) / (p1->mass + p2->mass);

    // Update velocities based on the impulse
    p1->velocity.x += impulse * p2->mass * nx;
    p1->velocity.y += impulse * p2->mass * ny;
    p2->velocity.x -= impulse * p1->mass * nx;
    p2->velocity.y -= impulse * p1->mass * ny;
  }
}

void step_simulation(Simulation sim) {
  SimulationStruct *sim_struct = (SimulationStruct *)sim;

  // Loop over each substep
  for (uint64_t substep = 0; substep < sim_struct->substeps; ++substep) {

    // Calculate forces
    for (uint64_t i = 0; i < sim_struct->particle_count; ++i) {
      Particle *p = &sim_struct->particles[i];

      // Reset force accumulator
      sim_struct->forces[i] = (Vector2D){0.0, 0.0};

      for (uint64_t j = 0; j < sim_struct->particle_count; ++j) {
        if (i == j)
          continue; // Skip self-interaction

        Particle *other = &sim_struct->particles[j];
        Vector2D force =
            calculate_force(p, other, sim_struct->gravitational_constant);
        sim_struct->forces[i].x += force.x;
        sim_struct->forces[i].y += force.y;
      }
    }

    // Integrate using Verlet method
    double substep_time = sim_struct->time_step / sim_struct->substeps;
    for (uint64_t i = 0; i < sim_struct->particle_count; ++i) {
      Particle *p = &sim_struct->particles[i];
      verlet_integration(p, sim_struct->forces[i], substep_time);
    }
  }

  if (sim_struct->enable_collisions) {
    for (uint64_t iter = 0; iter < sim_struct->collision_iterations; ++iter) {
      // Loop over each particle pair to check for collisions
      for (uint64_t i = 0; i < sim_struct->particle_count; ++i) {
        for (uint64_t j = i + 1; j < sim_struct->particle_count; ++j) {
          if (i == j)
            continue; // Skip self-interaction

          Particle *p1 = &sim_struct->particles[i];
          Particle *p2 = &sim_struct->particles[j];

          resolve_collision(p1, p2);
        }
      }
    }
  }
}

double calculate_distance(Vector2D *v1, Vector2D *v2) {
  double dx = v2->x - v1->x;
  double dy = v2->y - v1->y;
  return sqrt(dx * dx + dy * dy);
}

Vector2D calculate_force(Particle *p1, Particle *p2,
                         double gravitational_constant) {
  Vector2D force = {0.0, 0.0};
  Vector2D distance_vector = {p2->position.x - p1->position.x,
                              p2->position.y - p1->position.y};
  double distance = calculate_distance(&p1->position, &p2->position);
  double distance_squared = distance * distance;

  // TODO(lily): Scale the force down to zero as the overlap of the particles
  // increases instead of disabling the force
  double min_distance = p1->size + p2->size;
  if (distance > min_distance) {
    double force_magnitude =
        gravitational_constant * (p1->mass * p2->mass) / distance_squared;
    force.x = force_magnitude * (distance_vector.x / distance);
    force.y = force_magnitude * (distance_vector.y / distance);
  }

  return force;
}

void verlet_integration(Particle *p, Vector2D total_force, double time_step) {
  Vector2D acceleration = {total_force.x / p->mass, total_force.y / p->mass};

  // Update position
  p->position.x +=
      p->velocity.x * time_step + 0.5 * acceleration.x * time_step * time_step;
  p->position.y +=
      p->velocity.y * time_step + 0.5 * acceleration.y * time_step * time_step;

  // Update velocity
  p->velocity.x += acceleration.x * time_step;
  p->velocity.y += acceleration.y * time_step;
}

void toggle_collisions(bool enable) {
  (void)enable;
  // Enable or disable collision handling
}

Particle get_particle_state(Simulation sim, int particle_id) {
  SimulationStruct *sim_struct = (SimulationStruct *)sim;
  if (particle_id < 0 || (uint64_t)particle_id >= sim_struct->particle_count) {
    // Handle invalid particle_id
    Particle empty_particle = {0};
    return empty_particle;
  }
  return sim_struct->particles[particle_id];
}

int get_particles_by_area(Vector2D top_left, Vector2D bottom_right,
                          ParticleInfo *buffer, int max_count) {
  (void)top_left;
  (void)bottom_right;
  (void)buffer;
  (void)max_count;
  // Retrieve particles within the specified area
  return 0; // Placeholder return
}

// Adds a new particle to the simulation.
void add_particle(double mass, double size, Vector2D position,
                  Vector2D velocity) {
  (void)mass;
  (void)size;
  (void)position;
  (void)velocity;
  // Add a new particle with specified properties
}

// Removes a particle from the simulation.
void remove_particle(int particle_id) {
  (void)particle_id;
  // Remove the particle with the given ID
}

// Applies an external force to a specific particle.
void apply_force(int particle_id, Vector2D force) {
  (void)particle_id;
  (void)force;
  // Apply the specified force to the particle
}
void get_position_range(Simulation sim, Vector2D *min, Vector2D *max) {
  SimulationStruct *sim_struct = (SimulationStruct *)sim;
  *min = sim_struct->position_range[0];
  *max = sim_struct->position_range[1];
}

uint64_t get_particle_count(Simulation sim) {
  SimulationStruct *sim_struct = (SimulationStruct *)sim;
  return sim_struct->particle_count;
}

#endif // GRAVITY_C
