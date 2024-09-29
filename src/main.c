#include "gravity.h"
#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SPACE_GREY                                                             \
  CLITERAL(Color) { 43, 52, 60, 255 }

#define PARTICLE_COLOR_MIN                                                     \
  CLITERAL(Color) { 101, 115, 126, 255 }
#define PARTICLE_COLOR_MAX                                                     \
  CLITERAL(Color) { 207, 204, 222, 255 }

#define MASS_RANGE_MIN 100.0f
#define MASS_RANGE_MAX 2000.0f

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

Color interpolate_color(float mass, float mass_min, float mass_max);
void draw_simulation(Simulation sim);
Camera2D setup_camera(Vector2D pos_min, Vector2D pos_max);
void update_camera(Camera2D *camera);

int main(void) {
  Simulation sim = init_simulation((SimulationOptions){
      0.1,        // time step
      2,          // substeps
      true,       // collision enabled
      2,          // collision iterations
      6.67430e-4, // gravitational constant

      400,                  // particle count
      DISTRIBUTION_UNIFORM, // position distribution
      DISTRIBUTION_UNIFORM, // size distribution
      DISTRIBUTION_NORMAL,  // mass distribution

      {{-600.0, 600.0}, {600.0, -600.0}}, // position range
      {4.0, 8.0},                         // size range
      {MASS_RANGE_MIN, MASS_RANGE_MAX},   // mass range
  });
  assert(sim != NULL);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
             "raylib [core] example - basic window");

  SetGesturesEnabled(GESTURE_PINCH_IN | GESTURE_PINCH_OUT | GESTURE_DRAG);

  Vector2D pos_min, pos_max;
  get_position_range(sim, &pos_min, &pos_max);
  Camera2D camera = setup_camera(pos_min, pos_max);

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

void update_camera(Camera2D *camera) {
    // Handle zooming with pinch gesture
    if (IsGestureDetected(GESTURE_PINCH_IN) || IsGestureDetected(GESTURE_PINCH_OUT)) {
        Vector2 pinchVector = GetGesturePinchVector();
        float pinchRatio = 1.0f + (pinchVector.x + pinchVector.y) / 1000.0f; // Adjust sensitivity as needed
        camera->zoom *= pinchRatio;
        camera->zoom = Clamp(camera->zoom, 0.1f, 10.0f); // Limit zoom range
    }
    
    // Handle panning with two-finger drag
    if (IsGestureDetected(GESTURE_DRAG)) {
        Vector2 dragVector = GetGestureDragVector();
        camera->target.x -= dragVector.x / camera->zoom;
        camera->target.y -= dragVector.y / camera->zoom;
    }
}
