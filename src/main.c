#include "gravity.h"
#include "raylib.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SPACE_GREY                                                             \
  CLITERAL(Color) { 52, 61, 70, 255 }

#define PARTICLE_COLOR_MIN                                                     \
  CLITERAL(Color) { 101, 115, 126, 255 }
#define PARTICLE_COLOR_MAX                                                     \
  CLITERAL(Color) { 192, 197, 206, 255 }

#define MASS_RANGE_MIN 1.0f
#define MASS_RANGE_MAX 2.0f

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

void draw_simulation(Simulation *sim);
Camera2D setup_camera(Vector2D pos_min, Vector2D pos_max);

int main(void) {
  Simulation sim = init_simulation((SimulationOptions){
      0.1,   // time step
      1,     // substeps
      false, // collision enabled

      100,                 // particle count
      DISTRIBUTION_NORMAL, // position distribution
      DISTRIBUTION_NORMAL, // size distribution
      DISTRIBUTION_NORMAL, // mass distribution

      {{-100.0, 100.0}, {100.0, -100.0}}, // position range
      {1.0, 2.0},                         // size range
      {1.0, 2.0},                         // mass range
  });
  assert(sim != NULL);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
             "raylib [core] example - basic window");

  Vector2D pos_min, pos_max;
  get_position_range(sim, &pos_min, &pos_max);
  Camera2D camera = setup_camera(pos_min, pos_max);

  while (!WindowShouldClose()) {
    /*step_simulation(sim);*/

    BeginDrawing();
    BeginMode2D(camera);

    ClearBackground(SPACE_GREY);
    draw_simulation(sim);

    EndMode2D();
    EndDrawing();
  }

  deinit_simulation(sim);
  CloseWindow();

  return 0;
}

Camera2D setup_camera(Vector2D pos_min, Vector2D pos_max) {
  Camera2D camera = {0};
  camera.target = (Vector2){0.0f, 0.0f};
  camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};

  float range_x = 1.5 * (pos_max.x - pos_min.x);
  float range_y = 1.5 * (pos_max.y - pos_min.y);
  camera.zoom = (SCREEN_WIDTH / range_x < SCREEN_HEIGHT / range_y)
                    ? (SCREEN_WIDTH / range_x)
                    : (SCREEN_HEIGHT / range_y);

  return camera;
}

void draw_simulation(Simulation *sim) {
  uint64_t particle_count = get_particle_count(sim);
  for (uint64_t i = 0; i < particle_count; ++i) {
    Particle p = get_particle_state(sim, i);
    // Calculate the interpolation factor
    float factor =
        (p.mass - MASS_RANGE_MIN) / (MASS_RANGE_MAX - MASS_RANGE_MIN);

    // Interpolate color
    Color color = {
        (unsigned char)(PARTICLE_COLOR_MIN.r +
                        factor * (PARTICLE_COLOR_MAX.r - PARTICLE_COLOR_MIN.r)),
        (unsigned char)(PARTICLE_COLOR_MIN.g +
                        factor * (PARTICLE_COLOR_MAX.g - PARTICLE_COLOR_MIN.g)),
        (unsigned char)(PARTICLE_COLOR_MIN.b +
                        factor * (PARTICLE_COLOR_MAX.b - PARTICLE_COLOR_MIN.b)),
        255 // Assuming full opacity
    };

    // Draw particle with interpolated color
    DrawCircleV(
        (Vector2){
            p.position.x,
            p.position.y,
        },
        p.size, color);
  }
}
