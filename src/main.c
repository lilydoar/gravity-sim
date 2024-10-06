#include "user_interface.h"
#include "simulation.h"

#include "raylib.h"

int main(void)
{
    InitWindow(800, 450, "Simulation");
    SetTargetFPS(60);

    UIState state;
    init_ui_state(&state);

    Simulation simulation;
    simulation_init(&simulation);

    while (!WindowShouldClose())
    {
        double frame_time = GetFrameTime();

        simulation_update(&simulation, frame_time);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_ui(&state);
        EndDrawing();
    }

    simulation_deinit(&simulation);

    CloseWindow();
    return 0;
}
