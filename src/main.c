#include "gravity.h"
#include "raylib.h"

int main(void) {
  Simulation sim = init_simulation((SimulationOptions){
      false,                  // collision enabled
      100,                    // particle count
      DISTRIBUTION_UNDEFINED, // spawn distribution
  });

  InitWindow(800, 450, "raylib [core] example - basic window");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);
    EndDrawing();
  }

  deinit_simulation(sim);
  CloseWindow();

  return 0;
}
