#include "gravity.h"
#include "gravity_interactor.h"
#include "ui_handler.h"
#include "arena_allocator.h"
#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Forward declarations
Simulation init_simulation(SimulationOptions options);
void step_simulation(Simulation sim);
void deinit_simulation(Simulation sim);

// New function declarations
void draw_simulation(Simulation sim);
Camera2D setup_camera();
void update_camera(Camera2D *camera);

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

#define APP_ARENA_SIZE (1024 * 1024)  // 1 MB
#define FRAME_ARENA_SIZE (64 * 1024)  // 64 KB

Color interpolate_color(float mass, float mass_min, float mass_max);
void draw_simulation(Simulation sim);
Camera2D setup_camera();
void update_camera(Camera2D *camera);
void reset_camera(Camera2D *camera);

void draw_simulation(Simulation sim) {
    // Implement drawing logic here
    // For now, we'll just draw a placeholder circle
    DrawCircle(400, 300, 50, RED);
}

Camera2D setup_camera() {
    Camera2D camera = {0};
    camera.target = (Vector2){0, 0};
    camera.offset = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    return camera;
}

void update_camera(Camera2D *camera) {
    // Implement camera update logic here
    // For now, we'll just keep the camera static
}

int main(void) {
  ArenaAllocator* app_arena = create_arena(APP_ARENA_SIZE);
  ArenaAllocator* frame_arena = create_arena(FRAME_ARENA_SIZE);

  Simulation sim = init_simulation((SimulationOptions){
      .time_step = 0.5,
      .substeps = 5,
      .enable_collisions = true,
      .collision_iterations = 2,
      .gravitational_constant = 6.67430e-3,
      .particle_count = 400,
      .position_x_distribution = {.type = DISTRIBUTION_UNIFORM,
                                  .params.uniform = {.min = -2000.0,
                                                     .max = 2000.0}},
      .position_y_distribution = {.type = DISTRIBUTION_UNIFORM,
                                  .params.uniform = {.min = -2000.0,
                                                     .max = 2000.0}},
      .mass_distribution =
          {.type = DISTRIBUTION_NORMAL,
           .params.normal = {.mean = (MASS_RANGE_MIN + MASS_RANGE_MAX) / 2,
                             .stddev = (MASS_RANGE_MAX - MASS_RANGE_MIN) / 6}},
      .size_distribution = {.type = DISTRIBUTION_UNIFORM,
                            .params.uniform = {.min = 10.0, .max = 60.0}},
      .velocity_init_mode = VELOCITY_INIT_PERPENDICULAR_TO_ORIGIN,
      .velocity_magnitude_distribution = {
          .type = DISTRIBUTION_UNIFORM,
          .params.uniform = {.min = 0.0, .max = 8.0}}});
  assert(sim != NULL);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gravity Simulation");

  Camera2D camera = setup_camera();
  SimulationActor actor = init_simulation_interactor(app_arena);
  UIState ui_state = {0};

  while (!WindowShouldClose()) {
    reset_arena(frame_arena);
    
    handle_input(&ui_state, actor, frame_arena);
    
    for (int i = 0; i < MAX_ACTIONS_PER_FRAME; i++) {
      Action action = dequeue_action(&actor->queue);
      if (action.type == ACTION_EMPTY) break;
      apply_action(sim, action);
    }
    
    step_simulation(sim);
    update_camera(&camera);

    BeginDrawing();
    ClearBackground(SPACE_GREY);

    BeginMode2D(camera);
    draw_simulation(sim);
    EndMode2D();

    draw_ui(ui_state);
    DrawFPS(10, 10);
    EndDrawing();
  }

  deinit_simulation(sim);
  deinit_simulation_interactor(actor);
  destroy_arena(app_arena);
  destroy_arena(frame_arena);
  CloseWindow();

  return 0;
}

// ... (rest of the functions remain unchanged)
