#include "gravity.h"
#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SPACE_GREY                                                             \
  CLITERAL(Color) { 43, 52, 60, 255 }

#define PARTICLE_COLOR_MIN                                                     \
  CLITERAL(Color) { 93, 82, 116, 255 }
#define PARTICLE_COLOR_MAX                                                     \
  CLITERAL(Color) { 232, 204, 222, 255 }

#define MASS_RANGE_MIN 10000.0f
#define MASS_RANGE_MAX 50000.0f

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define CAMERA_MOVE_SPEED 5.0f
#define CAMERA_ZOOM_SPEED 0.05f

#define MIN_ZOOM 0.01f
#define MAX_ZOOM 10.0f
#define DEFAULT_ZOOM 0.05f

Color interpolate_color(float mass, float mass_min, float mass_max);
void draw_simulation(Simulation sim);
Camera2D setup_camera();
void update_camera(Camera2D *camera);
void reset_camera(Camera2D *camera);

int main(void) {
  Simulation sim = init_simulation((SimulationOptions){
    .time_step = 0.5,
    .substeps = 5,
    .enable_collisions = true,
    .collision_iterations = 2,
    .gravitational_constant = 6.67430e-3,
    .particle_count = 400,
    .position_x_distribution = {
        .type = DISTRIBUTION_UNIFORM,
        .params.uniform = {.min = -2000.0, .max = 2000.0}
    },
    .position_y_distribution = {
        .type = DISTRIBUTION_UNIFORM,
        .params.uniform = {.min = -2000.0, .max = 2000.0}
    },
    .mass_distribution = {
        .type = DISTRIBUTION_NORMAL,
        .params.normal = {.mean = (MASS_RANGE_MIN + MASS_RANGE_MAX) / 2, .stddev = (MASS_RANGE_MAX - MASS_RANGE_MIN) / 6}
    },
    .size_distribution = {
        .type = DISTRIBUTION_UNIFORM,
        .params.uniform = {.min = 10.0, .max = 60.0}
    },
    .velocity_x_distribution = {
        .type = DISTRIBUTION_UNIFORM,
        .params.uniform = {.min = -8.0, .max = 8.0}
    },
    .velocity_y_distribution = {
        .type = DISTRIBUTION_UNIFORM,
        .params.uniform = {.min = -8.0, .max = 8.0}
    }
  });
  assert(sim != NULL);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gravity Simulation");

  Camera2D camera = setup_camera();

  while (!WindowShouldClose()) {
    step_simulation(sim);
    update_camera(&camera);

    BeginDrawing();
    ClearBackground(SPACE_GREY);

    BeginMode2D(camera);
    draw_simulation(sim);
    EndMode2D();

    DrawFPS(10, 10);
    EndDrawing();
  }

  deinit_simulation(sim);
  CloseWindow();

  return 0;
}

Color interpolate_color(float mass, float mass_min, float mass_max) {
  // Calculate the interpolation factor
  float factor = (mass - mass_min) / (mass_max - mass_min);

  // Interpolate color
  return (Color){
      (unsigned char)(PARTICLE_COLOR_MIN.r +
                      factor * (PARTICLE_COLOR_MAX.r - PARTICLE_COLOR_MIN.r)),
      (unsigned char)(PARTICLE_COLOR_MIN.g +
                      factor * (PARTICLE_COLOR_MAX.g - PARTICLE_COLOR_MIN.g)),
      (unsigned char)(PARTICLE_COLOR_MIN.b +
                      factor * (PARTICLE_COLOR_MAX.b - PARTICLE_COLOR_MIN.b)),
      255 // Assuming full opacity
  };
}

Camera2D setup_camera() {
  Camera2D camera = {0};
  camera.target = (Vector2){0.0f, 0.0f};
  camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
  camera.zoom = DEFAULT_ZOOM;
  return camera;
}

void update_camera(Camera2D *camera) {
  // Handle zooming with mouse wheel
  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);
    camera->offset = GetMousePosition();
    camera->target = mouseWorldPos;
    camera->zoom += (wheel * CAMERA_ZOOM_SPEED);
  }

  // Handle keyboard controls
  if (IsKeyDown(KEY_W))
    camera->target.y -= CAMERA_MOVE_SPEED / camera->zoom;
  if (IsKeyDown(KEY_S))
    camera->target.y += CAMERA_MOVE_SPEED / camera->zoom;
  if (IsKeyDown(KEY_A))
    camera->target.x -= CAMERA_MOVE_SPEED / camera->zoom;
  if (IsKeyDown(KEY_D))
    camera->target.x += CAMERA_MOVE_SPEED / camera->zoom;

  // Reset camera
  if (IsKeyPressed(KEY_R)) {
    reset_camera(camera);
  }

  // Clamp zoom
  camera->zoom = Clamp(camera->zoom, MIN_ZOOM, MAX_ZOOM);
}

void reset_camera(Camera2D *camera) {
  camera->target = (Vector2){0.0f, 0.0f};
  camera->offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
  camera->zoom = DEFAULT_ZOOM;
}

void draw_simulation(Simulation sim) {
  uint64_t particle_count = get_particle_count(sim);
  for (uint64_t i = 0; i < particle_count; ++i) {
    Particle p = get_particle_state(sim, i);
    // Interpolate color based on mass
    Color color = interpolate_color(p.mass, MASS_RANGE_MIN, MASS_RANGE_MAX);

    // Draw particles
    DrawCircleV(
        (Vector2){
            p.position.x,
            p.position.y,
        },
        p.size, color);
  }
}
