#include "gravity.h"
#include "raylib.h"
#include <assert.h>
#include <stdlib.h>

#define SPACE_GREY                                                             \
  CLITERAL(Color) { 52, 61, 70, 255 }

void draw_simulation(Simulation *sim);

int main(void) {
  Simulation sim = init_simulation((SimulationOptions){
      false,                   // collision enabled
      100,                     // particle count
      DISTRIBUTION_UNDEFINED,  // position distribution
      DISTRIBUTION_UNDEFINED,  // size distribution
      DISTRIBUTION_UNDEFINED,  // mass distribution
      {{0.0, 0.0}, {100.0, 100.0}}, // position range
      {1.0, 2.0},              // size range
      {1.0, 2.0},              // mass range
      0.1,                     // time step
      1,                       // substeps
  });
  assert(sim != NULL);

  InitWindow(800, 450, "raylib [core] example - basic window");

  while (!WindowShouldClose()) {
    step_simulation(sim);

    BeginDrawing();
    ClearBackground(SPACE_GREY);
    draw_simulation(sim);
    EndDrawing();
  }

  deinit_simulation(sim);
  CloseWindow();

  return 0;
}

void draw_simulation(Simulation *sim) {}
