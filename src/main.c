#include "gravity.h"
#include "raylib.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SPACE_GREY                                                             \
  CLITERAL(Color) { 52, 61, 70, 255 }

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

void draw_simulation(Simulation *sim);

int main(void) {
  Simulation sim = init_simulation((SimulationOptions){
      0.1,   // time step
      1,     // substeps
      false, // collision enabled

      100,                    // particle count
      DISTRIBUTION_UNDEFINED, // position distribution
      DISTRIBUTION_UNDEFINED, // size distribution
      DISTRIBUTION_UNDEFINED, // mass distribution

      {{-100.0, 100.0}, {100.0, -100.0}}, // position range
      {10.0, 20.0},                       // size range
      {1.0, 2.0},                         // mass range
  });
  assert(sim != NULL);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
             "raylib [core] example - basic window");

  // Initialize camera
  Camera2D camera = {0};
  camera.target = (Vector2){0.0f, 0.0f};
  camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};

  // Retrieve position range and calculate zoom to fit the entire simulation on screen
  Vector2D min, max;
  get_position_range(sim, &min, &max);
  float range_x = max.x - min.x;
  float range_y = max.y - min.y;
  camera.zoom = fminf(SCREEN_WIDTH / range_x, SCREEN_HEIGHT / range_y);

  while (!WindowShouldClose()) {
    step_simulation(sim);

    BeginMode2D(camera);
    ClearBackground(SPACE_GREY);
    draw_simulation(sim);
    EndMode2D();
  }

  deinit_simulation(sim);
  CloseWindow();

  return 0;
}

void draw_simulation(Simulation *sim) {
  uint64_t particle_count = get_particle_count(sim);
  for (uint64_t i = 0; i < particle_count; ++i) {
    Particle p = get_particle_state(sim, i);
    // Calculate screen position
    /*float screen_x = (float)(SCREEN_WIDTH) / 2 + p.position.x;*/
    /*float screen_y = (float)(SCREEN_HEIGHT) / 2 + p.position.y;*/
    float screen_x = 0.0;
    float screen_y = 0.0;
    // Draw particle as a red circle
    DrawCircleV((Vector2){screen_x, screen_y}, p.size, RED);
  }
}
