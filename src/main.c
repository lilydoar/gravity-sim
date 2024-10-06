#include "arena_allocator.h"
#include "simulation.h"
#include "raylib.h"

#include <math.h>

#define APP_ARENA_SIZE (1024 * 1024)   // 1 MB
#define FRAME_ARENA_SIZE (1024 * 1024) // 1 MB

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PARTICLE_DENSITY 1

// Forward declarations
Camera2D setup_camera();
void update_camera(Camera2D *camera);
void draw_simulation(Simulation *simulation);

float calculate_particle_radius(Particle p)
{
  float area = p.mass / PARTICLE_DENSITY;
  return sqrt(area / M_PI);
}

int main(void)
{
  ArenaAllocator *simulation_arena = init_arena(APP_ARENA_SIZE);
  ArenaAllocator *frame_arena = init_arena(FRAME_ARENA_SIZE);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gravity Simulation");

  Simulation simulation = simulation_init();
  Camera2D camera = setup_camera();

  while (!WindowShouldClose())
  {
    reset_arena(frame_arena);

    double time_step = GetFrameTime();
    simulation_update(&simulation, frame_arena, time_step);

    update_camera(&camera);

    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode2D(camera);
    draw_simulation(&simulation);
    EndMode2D();

    DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}

// Setup the camera
// Set origin to the center of the screen
Camera2D setup_camera()
{
  Camera2D camera;
  camera.offset = (Vector2){(float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2};
  camera.target = (Vector2){0, 0};
  camera.rotation = 0;
  camera.zoom = 1;
  return camera;
}

// Update the camera
// Zoom in and out with the mouse wheel
// Move the camera with the arrow keys
void update_camera(Camera2D *camera)
{
  float move_speed = 100.0f;
  if (IsKeyDown(KEY_UP))
    camera->target.y -= move_speed * GetFrameTime();
  if (IsKeyDown(KEY_DOWN))
    camera->target.y += move_speed * GetFrameTime();
  if (IsKeyDown(KEY_LEFT))
    camera->target.x -= move_speed * GetFrameTime();
  if (IsKeyDown(KEY_RIGHT))
    camera->target.x += move_speed * GetFrameTime();

  if (IsKeyDown(KEY_E))
    camera->zoom += GetFrameTime();
  if (IsKeyDown(KEY_Q))
    camera->zoom -= GetFrameTime();
  if (camera->zoom < 0)
    camera->zoom = 0;
}

// Draw the simulation
void draw_simulation(Simulation *simulation)
{
  for (uint64_t i = 0; i < simulation->particle_count; i++)
  {
    Particle *p = &simulation->particles[i];
    Vector2 position = {p->position.x, p->position.y};
    DrawCircleV(position, calculate_particle_radius(*p), WHITE);
  }
}