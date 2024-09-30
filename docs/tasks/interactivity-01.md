# Interactivity Implementation Plan

## Overview

This plan outlines the steps to implement an interactive particle selection and modification system for the gravity simulation. The system will allow users to select particles using rectangular or circular areas and apply various actions to them.

## Components

### 1. Arena Allocator

Implement a basic arena allocator for efficient memory management.

File: `include/arena_allocator.h`

```c
typedef struct ArenaAllocator ArenaAllocator;

ArenaAllocator* create_arena(size_t size);
void* arena_alloc(ArenaAllocator* arena, size_t size);
void reset_arena(ArenaAllocator* arena);
void destroy_arena(ArenaAllocator* arena);
```

### 2. Action and Selection System

Define structures and functions for particle selection and actions.

File: `include/gravity_interactor.h`

```c
typedef enum {
    SELECTION_RECTANGLE,
    SELECTION_CIRCLE
} SelectionType;

typedef struct {
    SelectionType type;
    union {
        struct { Vector2D top_left; Vector2D bottom_right; } rectangle;
        struct { Vector2D center; float radius; } circle;
    } shape;
} ParticleSelection;

typedef enum {
    ACTION_MAKE_STATIC,
    ACTION_MAKE_NON_STATIC,
    ACTION_REMOVE_FROM_SIM,
    ACTION_INCREASE_DENSITY,
    ACTION_DECREASE_DENSITY
} ActionType;

typedef struct {
    ActionType type;
    ParticleSelection selection;
} Action;

Action create_action(ArenaAllocator* frame_arena, ActionType type, ParticleSelection selection);
void apply_action(Simulation sim, Action action);

#define MAX_ACTIONS_PER_FRAME 10

typedef struct {
    Action actions[MAX_ACTIONS_PER_FRAME];
    int count;
} ActionQueue;

void enqueue_action(ActionQueue* queue, Action action);
Action dequeue_action(ActionQueue* queue);
```

### 3. UI and Interaction

Handle user input and draw the UI.

File: `include/ui_handler.h`

```c
typedef struct {
    bool is_selecting;
    Vector2D start_pos;
    Vector2D current_pos;
    SelectionType current_selection_type;
} UIState;

void handle_input(UIState* state, SimulationActor actor, ArenaAllocator* frame_arena);
void draw_ui(UIState state);
```

### 4. Simulation Interface

Add functions to query particles within geometric shapes.

File: `include/gravity.h`

```c
int get_particles_in_rectangle(Simulation sim, Vector2D top_left, Vector2D bottom_right, int* particle_ids, int max_count);
int get_particles_in_circle(Simulation sim, Vector2D center, float radius, int* particle_ids, int max_count);
```

### 5. Main Loop Structure

Update the main loop to incorporate the new systems.

File: `src/main.c`

```c
int main(void) {
    // ... (initialization code)
    
    ArenaAllocator* app_arena = create_arena(APP_ARENA_SIZE);
    ArenaAllocator* frame_arena = create_arena(FRAME_ARENA_SIZE);
    
    SimulationActor actor = init_simulation_interactor(app_arena);
    UIState ui_state = {0};
    
    while (!WindowShouldClose()) {
        reset_arena(frame_arena);
        
        handle_input(&ui_state, actor, frame_arena);
        
        for (int i = 0; i < MAX_ACTIONS_PER_FRAME; i++) {
            Action action = dequeue_action(actor->queue);
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
    
    // ... (cleanup code)
}
```

## Implementation Steps

1. Create the arena allocator:

   - Implement functions in `src/arena_allocator.c`
   - Test basic functionality

1. Update the gravity interactor:

   - Implement action creation and application functions in `src/gravity_interactor.c`
   - Implement queue operations for actions

1. Implement UI handling:

   - Create `src/ui_handler.c` and implement input handling and UI drawing functions
   - Integrate with raylib for drawing and input

1. Update the gravity simulation:

   - Implement particle selection functions in `src/gravity.c`
   - Ensure these functions are efficient and can be optimized later

1. Modify the main loop:

   - Update `src/main.c` to use the new structures and functions
   - Ensure proper integration of all components

1. Testing and refinement:

   - Test the entire system for proper functionality
   - Refine and optimize as necessary

## Design Notes

1. The arena allocator provides scoped memory management. The app_arena is used for long-lived objects, while the frame_arena is reset each frame for temporary allocations.

1. The ParticleSelection struct allows for flexible selection shapes, easily extensible to other shapes in the future.

1. The Action struct combines the action type with the selection, allowing for efficient processing.

1. The UI state is separate from the simulation actor, allowing for clear separation of concerns.

1. The simulation interface (get_particles_in_rectangle, get_particles_in_circle) provides a clean abstraction that can be optimized with spatial partitioning in the future without changing the interface.

1. The main loop structure ensures that actions are applied within a single frame, with a cap on the number of actions processed per frame.

1. The draw_ui function encapsulates all UI rendering, including selection areas.

## Makefile Updates

Update the Makefile to include the new source files:

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c17
LFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
INCLUDE = -Iinclude/
LIB = -Llib/ -lraylib
TARGET = bin/gravity-sim
SOURCES = src/main.c src/gravity.c src/gravity_interactor.c src/arena_allocator.c src/ui_handler.c

all: $(TARGET)

$(TARGET): $(SOURCES)
    $(CC) $(CFLAGS) $(LFLAGS) $(INCLUDE) $(LIB) -o $(TARGET) $(SOURCES)

run: $(TARGET)
    ./$(TARGET)

clean:
    rm $(TARGET)
```

This plan provides a structured approach to implementing the interactivity features in the gravity simulation. Follow these steps, implementing each component and integrating them into the main application. Test thoroughly at each stage to ensure proper functionality.
