#include "arena_allocator.h"
#include "gravity.h"
#include "gravity_interactor.h"
#include "logging.h"
#include "raylib.h"
#include "raymath.h"
// #include "ui_handler.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ui.h"

// Use doubles for cglm types
#define CGLM_USE_DOUBLE

#define MASS_RANGE_MIN 10000.0f
#define MASS_RANGE_MAX 50000.0f

// New function declarations
Camera2D camera; // Define the camera globally

void draw_simulation(Simulation sim);
Camera2D setup_camera();
void update_camera(Camera2D *camera);

#define SPACE_GREY \
  CLITERAL(Color) { 43, 52, 60, 255 }

#define PARTICLE_COLOR_MIN \
  CLITERAL(Color) { 93, 82, 116, 255 }
#define PARTICLE_COLOR_MAX \
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

#define APP_ARENA_SIZE (1024 * 1024) // 1 MB
#define FRAME_ARENA_SIZE (64 * 1024) // 64 KB

Color interpolate_color(float mass, float mass_min, float mass_max);
void draw_simulation(Simulation sim);
Camera2D setup_camera();
void update_camera(Camera2D *camera);
void reset_camera(Camera2D *camera);

void draw_simulation(Simulation sim)
{
  uint64_t particle_count = simulation_get_particle_count(sim);
  for (uint64_t i = 0; i < particle_count; i++)
  {
    SimulationParticle p = simulation_get_particle_state(sim, i);
    Color particle_color;
    vec2s position;
    float radius;

    switch (p.mode)
    {
    case PARTICLE_MODE_STATIC:
    {
      particle_color = (Color){127, 255, 212, 255};
      position = p.params.STATIC.position;
      radius = p.params.STATIC.radius;
    }
    break;
    case PARTICLE_MODE_VERLET:
    {
      float normalized_mass = (p.params.VERLET.mass - MASS_RANGE_MIN) /
                              (MASS_RANGE_MAX - MASS_RANGE_MIN);
      particle_color = interpolate_color(normalized_mass, 0, 1);
      position = p.params.VERLET.position;
      radius = p.params.VERLET.radius;
    }
    break;
    }
    DrawCircle(round(position.x), round(position.y), radius, particle_color);
  }
}

Camera2D setup_camera()
{
  Camera2D camera = {0};
  camera.target = (Vector2){0, 0};
  camera.offset = (Vector2){(float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2};
  camera.rotation = 0.0f;
  camera.zoom = DEFAULT_ZOOM;
  return camera;
}

void update_camera(Camera2D *camera)
{
  if (IsKeyDown(KEY_W))
    camera->target.y -= CAMERA_MOVE_SPEED / camera->zoom;
  if (IsKeyDown(KEY_S))
    camera->target.y += CAMERA_MOVE_SPEED / camera->zoom;
  if (IsKeyDown(KEY_A))
    camera->target.x -= CAMERA_MOVE_SPEED / camera->zoom;
  if (IsKeyDown(KEY_D))
    camera->target.x += CAMERA_MOVE_SPEED / camera->zoom;

  float wheel = GetMouseWheelMove();
  if (wheel != 0)
  {
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);
    camera->offset = GetMousePosition();
    camera->target = mouseWorldPos;
    camera->zoom += wheel * CAMERA_ZOOM_SPEED * camera->zoom;
    if (camera->zoom < MIN_ZOOM)
      camera->zoom = MIN_ZOOM;
    if (camera->zoom > MAX_ZOOM)
      camera->zoom = MAX_ZOOM;
  }
}

void reset_camera(Camera2D *camera)
{
  camera->target = (Vector2){0, 0};
  camera->zoom = DEFAULT_ZOOM;
}

Color interpolate_color(float t, float t_min, float t_max)
{
  float normalized_t = (t - t_min) / (t_max - t_min);
  return (Color){
      PARTICLE_COLOR_MIN.r +
          (PARTICLE_COLOR_MAX.r - PARTICLE_COLOR_MIN.r) * normalized_t,
      PARTICLE_COLOR_MIN.g +
          (PARTICLE_COLOR_MAX.g - PARTICLE_COLOR_MIN.g) * normalized_t,
      PARTICLE_COLOR_MIN.b +
          (PARTICLE_COLOR_MAX.b - PARTICLE_COLOR_MIN.b) * normalized_t,
      255};
}

int main(void)
{
  ArenaAllocator *app_arena = init_arena(APP_ARENA_SIZE);
  ArenaAllocator *frame_arena = init_arena(FRAME_ARENA_SIZE);

  Simulation sim = init_simulation((SimulationOptions){
      .time_step = 0.01,
      .substeps = 10,
      .collision_iterations = 2,
      .gravitational_constant = 6.67430e-2,
      .particle_count = 250,
      .position_x_distribution = {.type = DISTRIBUTION_UNIFORM,
                                  .params.uniform = {.min = -1000.0,
                                                     .max = 1000.0}},
      .position_y_distribution = {.type = DISTRIBUTION_UNIFORM,
                                  .params.uniform = {.min = -1000.0,
                                                     .max = 1000.0}},
      .mass_distribution =
          {.type = DISTRIBUTION_NORMAL,
           .params.normal = {.mean = (MASS_RANGE_MIN + MASS_RANGE_MAX) / 2,
                             .stddev = (MASS_RANGE_MAX - MASS_RANGE_MIN) / 6}},
      .size_distribution = {.type = DISTRIBUTION_UNIFORM,
                            .params.uniform = {.min = 5.0, .max = 30.0}},
      .initial_particle_mode = PARTICLE_MODE_VERLET,
      .velocity_init_mode = VELOCITY_INIT_PERPENDICULAR_TO_ORIGIN,
      .velocity_magnitude_distribution = {
          .type = DISTRIBUTION_UNIFORM,
          .params.uniform = {.min = 0.0, .max = 20.0}}}); // Increased max velocity
  assert(sim != NULL);

  // Log initial particle modes
  uint64_t particle_count = simulation_get_particle_count(sim);
  for (uint64_t i = 0; i < particle_count; i++)
  {
    SimulationParticle p = simulation_get_particle_state(sim, i);
    TRACE_LOG("Initial mode of particle %llu: %d", i, p.mode);
  }

  // Set the current log level to TRACE
  set_log_level(LOG_LEVEL_DEBUG);

  UIState ui_state;
  init_ui_state(&ui_state, app_arena, frame_arena);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gravity Simulation");
  SetTargetFPS(60);

  camera = setup_camera();
  SimulationActor actor = init_simulation_interactor(app_arena, sim);
  // UIState ui_state;
  // init_ui_state(&ui_state);

  while (!WindowShouldClose())
  {
    reset_arena(frame_arena);

    // handle_input(&ui_state, actor, frame_arena);

    for (int i = 0; i < MAX_ACTIONS_PER_FRAME; i++)
    {
      Action action = dequeue_action(&actor->queue);
      if (action.type == ACTION_EMPTY)
        break;
      apply_action(sim, action);
    }

    step_simulation(sim);
    update_camera(&camera);

    BeginDrawing();
    ClearBackground(SPACE_GREY);

    BeginMode2D(camera);
    step_simulation(sim);

    draw_simulation(sim);
    EndMode2D();

    // draw_ui(ui_state, actor, frame_arena);
    draw_ui(&ui_state);
    DrawFPS(10, 10);
    TRACE_LOG("Frame completed");
    EndDrawing();

    if (IsKeyPressed(KEY_R))
    {
      reset_camera(&camera);
    }
  }

  deinit_simulation(sim);
  deinit_simulation_interactor(actor);
  deinit_ui_state(&ui_state);
  deinit_arena(app_arena);
  deinit_arena(frame_arena);
  CloseWindow();

  return 0;
}
