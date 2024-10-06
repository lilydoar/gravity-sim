#include "arena_allocator.h"
#include "raylib.h"
#include "raymath.h"
#include "simulation.h"
#include "user_input.h"
#include "user_interface.h"
#include "vector.h"

#include <math.h>
#include <stdio.h>

#define APP_ARENA_SIZE (1024 * 1024)   // 1 MB
#define FRAME_ARENA_SIZE (1024 * 1024) // 1 MB

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PARTICLE_DENSITY 1

#define CAMERA_MOVE_SPEED 200.0f
#define CAMERA_ZOOM_SPEED 0.1f
#define CAMERA_MIN_ZOOM 0.1f
#define CAMERA_MAX_ZOOM 10.0f

// Forward declarations
Camera2D camera_setup();
void simulation_apply_input(Simulation *simulation, ArenaAllocator *sim_arena,
                            UserInput input, UIState state, Camera2D camera);
void simulation_draw(Simulation *simulation);

// Calculate the radius of a particle based on its mass
float calculate_particle_radius(double mass) {
  float area = mass / PARTICLE_DENSITY;
  return sqrt(area / M_PI);
}
// Calculate the mass of a particle based on its radius
float calculate_particle_mass(float radius) {
  float area = M_PI * radius * radius;
  return area * PARTICLE_DENSITY;
}

// Convert a screen position to a simulation position
Vec2 screen_to_simulation_space(Camera2D camera, Vector2 screen_position) {
  Vector2 world_pos = GetScreenToWorld2D(screen_position, camera);
  return (Vec2){world_pos.x, world_pos.y};
}
// Convert a simulation position to a screen position
Vector2 simulation_to_screen_space(Camera2D camera, Vec2 position) {
  return GetWorldToScreen2D((Vector2){position.x, position.y}, camera);
}

int main(void) {
  ArenaAllocator *sim_arena = init_arena(APP_ARENA_SIZE);
  ArenaAllocator *frame_arena = init_arena(FRAME_ARENA_SIZE);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gravity Simulation");

  Simulation simulation = simulation_init(6.67430e-6);
  Camera2D camera = camera_setup();

  UIState ui_state = (UIState){.current_tool = UI_TOOL_SELECT,
                               .select_mode = SELECT_MODE_BOX,
                               .move_mode = MOVE_MODE_POSITION,
                               .spawn_mode = SPAWN_MODE_SINGLE};
  UserInput user_input = (UserInput){.mouse_left_pressed = false,
                                     .mouse_start = (Vector2){0, 0},
                                     .mouse_current = (Vector2){0, 0}};

  while (!WindowShouldClose()) {
    reset_arena(frame_arena);

    double time_step = GetFrameTime();
    collect_input(&user_input);
    
    update_camera(&camera, time_step);
    
    simulation_apply_input(&simulation, sim_arena, user_input, ui_state,
                           camera);
    simulation_update(&simulation, frame_arena, time_step);

    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode2D(camera);
    simulation_draw(&simulation);
    EndMode2D();

    bool button_pressed = draw_ui(&ui_state);
    if (!button_pressed) {
      draw_tool(&ui_state, &user_input);
    }

    DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}

// Setup the camera
// Set origin to the center of the screen
Camera2D camera_setup() {
  Camera2D camera;
  camera.offset = (Vector2){(float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2};
  camera.target = (Vector2){0, 0};
  camera.rotation = 0;
  camera.zoom = 1.0f;
  return camera;
}

void update_camera(Camera2D *camera, float delta_time) {
  // Camera pan with WASD keys
  if (IsKeyDown(KEY_W)) camera->target.y -= CAMERA_MOVE_SPEED * delta_time / camera->zoom;
  if (IsKeyDown(KEY_S)) camera->target.y += CAMERA_MOVE_SPEED * delta_time / camera->zoom;
  if (IsKeyDown(KEY_A)) camera->target.x -= CAMERA_MOVE_SPEED * delta_time / camera->zoom;
  if (IsKeyDown(KEY_D)) camera->target.x += CAMERA_MOVE_SPEED * delta_time / camera->zoom;

  // Camera zoom with scroll wheel, relative to mouse position
  float wheel = GetMouseWheelMove();
  if (wheel != 0) {
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);
    
    camera->offset = GetMousePosition();
    camera->target = mouseWorldPos;

    camera->zoom += (wheel * CAMERA_ZOOM_SPEED * camera->zoom);
    camera->zoom = Clamp(camera->zoom, CAMERA_MIN_ZOOM, CAMERA_MAX_ZOOM);

    Vector2 newMouseWorldPos = GetScreenToWorld2D(GetMousePosition(), *camera);
    camera->target = Vector2Add(camera->target, Vector2Subtract(mouseWorldPos, newMouseWorldPos));
  }
}

// Apply commands to the simulation
// Construct commands from UI state and user input
void simulation_apply_input(Simulation *simulation, ArenaAllocator *arena,
                            UserInput input, UIState state, Camera2D camera) {
  if (state.current_tool == UI_TOOL_SPAWN) {
    if (input.mouse_left_released) {
      Vec2 start = screen_to_simulation_space(camera, input.mouse_start);
      Vec2 end = screen_to_simulation_space(camera, input.mouse_current);
      simulation_new_particle(
          simulation, arena,
          (Particle){.position = (Vec2){start.x, start.y},
                     .velocity = (Vec2){0, 0},
                     .mass = calculate_particle_mass(vec2_dist(start, end)),
                     .radius = vec2_dist(start, end)});
      printf("spawned particle with mass %f\n",
             simulation->particles[simulation->particle_count - 1].mass);
    }
  }
}

// Draw the simulation
void simulation_draw(Simulation *simulation) {
  for (uint64_t i = 0; i < simulation->particle_count; i++) {
    Particle *p = &simulation->particles[i];
    Vector2 position = {p->position.x, p->position.y};
    DrawCircleV(position, calculate_particle_radius(p->mass), WHITE);
  }
}
