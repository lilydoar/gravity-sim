#include "arena_allocator.h"
#include "simulation.h"
#include "raylib.h"

// Forward declarations
Camera2D setup_camera();
void draw_simulation(Simulation *simulation);

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define APP_ARENA_SIZE (1024 * 1024) // 1 MB
#define FRAME_ARENA_SIZE (1024 * 1024) // 1 MB

int main(void) {
  ArenaAllocator *app_arena = init_arena(APP_ARENA_SIZE);
  ArenaAllocator *frame_arena = init_arena(FRAME_ARENA_SIZE);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gravity Simulation");

  Camera2D camera = setup_camera();

  Simulation simulation = simulation_init();

  while (!WindowShouldClose()) {
    reset_arena(frame_arena);

    double time_step = GetFrameTime();
    simulation_update(&simulation, frame_arena, time_step);

    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode2D(camera);
    draw_simulation(&simulation);
    EndMode2D();

    DrawFPS(10, 10);
    EndDrawing();
  }

  simulation_deinit(&simulation);
  deinit_arena(app_arena);
  deinit_arena(frame_arena);
  CloseWindow();

  return 0;
}

Camera2D setup_camera() {
  Camera2D camera;
  camera.offset = (Vector2){(float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2};
  camera.target = (Vector2){0, 0};
  camera.rotation = 0.0;
  camera.zoom = 1.0;
  return camera;
}

void draw_simulation(Simulation *simulation) {
  for (uint64_t i = 0; i < simulation->particle_count; i++) {
    Particle *p = &simulation->particles[i];
    DrawCircleV((Vector2){p->position.x, p->position.y}, 10, WHITE);
  }
}