# Debugging Plan for Particle Selection and Static Setting

## 1. Add Logging

First, let's add some logging to trace the execution flow and state changes.

### Task 1.1: Add debug logging to gravity_interactor.c

Add the following at the top of `src/gravity_interactor.c`:

```c
#include <stdio.h>
#define DEBUG_LOG(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__)
```

Then, add logging statements in key functions:

```c
void apply_action(Simulation sim, Action action) {
    DEBUG_LOG("Applying action of type %d", action.type);
    if (action.type == ACTION_MAKE_STATIC) {
        if (action.selection.type == SELECTION_RECTANGLE) {
            // ... existing code ...
            DEBUG_LOG("Rectangle selection: (%f, %f) to (%f, %f)", 
                actual_top_left.x, actual_top_left.y, 
                actual_bottom_right.x, actual_bottom_right.y);
            DEBUG_LOG("Found %d particles in selection", count);
            // ... rest of the function ...
        }
    }
}
```

### Task 1.2: Add debug logging to ui_handler.c

Similarly, add logging to `src/ui_handler.c`:

```c
void handle_input(UIState* state, SimulationActor actor, ArenaAllocator* frame_arena) {
    // ... existing code ...
    if (state->is_selecting) {
        state->current_pos = GetMousePosition();
        DEBUG_LOG("Selection in progress: (%f, %f) to (%f, %f)", 
            state->start_pos.x, state->start_pos.y, 
            state->current_pos.x, state->current_pos.y);
        
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            // ... existing code ...
            DEBUG_LOG("Selection completed, creating action of type %d", ACTION_MAKE_STATIC);
            // ... rest of the function ...
        }
    }
}
```

## 2. Add Unit Tests

Create a new file `tests/test_gravity_interactor.c` with the following content:

```c
#include "gravity_interactor.h"
#include "gravity.h"
#include <assert.h>
#include <stdio.h>

// Mock functions for Simulation
Simulation mock_sim;
Particle mock_particles[10];

Particle get_particle_state(Simulation sim, int particle_id) {
    return mock_particles[particle_id];
}

void set_particle_state(Simulation sim, int particle_id, Particle p) {
    mock_particles[particle_id] = p;
}

int get_particles_in_rectangle(Simulation sim, Vector2D top_left, Vector2D bottom_right, int* particle_ids, int max_count) {
    // Simple mock: return 2 particles
    particle_ids[0] = 0;
    particle_ids[1] = 1;
    return 2;
}

void test_apply_action() {
    // Initialize mock particles
    for (int i = 0; i < 10; i++) {
        mock_particles[i].is_static = false;
    }

    // Create a test action
    Action action = {
        .type = ACTION_MAKE_STATIC,
        .selection = {
            .type = SELECTION_RECTANGLE,
            .shape = {
                .rectangle = {
                    .top_left = {0, 0},
                    .bottom_right = {100, 100}
                }
            }
        }
    };

    // Apply the action
    apply_action(mock_sim, action);

    // Check if particles were set to static
    assert(mock_particles[0].is_static == true);
    assert(mock_particles[1].is_static == true);

    printf("test_apply_action passed\n");
}

int main() {
    test_apply_action();
    return 0;
}
```

## 3. Modify Makefile

Update the Makefile to include the new test:

```makefile
# ... existing content ...

TEST_SOURCES = tests/test_gravity_interactor.c src/gravity_interactor.c src/arena_allocator.c
TEST_TARGET = bin/test_gravity_interactor

$(TEST_TARGET): $(TEST_SOURCES)
    $(CC) $(CFLAGS) $(INCLUDE) -o $(TEST_TARGET) $(TEST_SOURCES)

test: $(TEST_TARGET)
    ./$(TEST_TARGET)

.PHONY: test
```

## 4. Developer Inspection

Ask the developer to inspect the following areas:

1. Check if the `apply_action` function in `src/gravity_interactor.c` is being called when a selection is made.
2. Verify that the `get_particles_in_rectangle` function in `src/gravity.c` is correctly identifying particles within the selection.
3. Ensure that the `set_particle_state` function in `src/gravity.c` is actually modifying the particle's `is_static` property.

## 5. Visual Debugging

Add visual feedback for static particles:

In `src/main.c`, modify the `draw_simulation` function:

```c
void draw_simulation(Simulation sim) {
    // ... existing code ...
    for (uint64_t i = 0; i < particle_count; i++) {
        Particle p = get_particle_state(sim, i);
        Color particle_color;
        if (p.is_static) {
            particle_color = RED;  // Change color for static particles
        } else {
            // ... existing color calculation ...
        }
        DrawCircle((int)p.position.x, (int)p.position.y, p.size, particle_color);
    }
}
```

## 6. Run Tests and Debug

1. Compile and run the tests:
   ```
   make test
   ```

2. Run the simulation with debug output:
   ```
   make run 2> debug.log
   ```

3. Analyze the debug.log file for any issues in the selection and action application process.

## 7. Interactive Debugging

If the above steps don't reveal the issue:

1. Use a debugger like GDB to set breakpoints in key functions (`apply_action`, `handle_input`, etc.).
2. Step through the code during a selection operation to see exactly where the expected behavior diverges from the actual behavior.

## 8. Report Findings

After completing these steps, compile your findings into a report, including:
- Results of unit tests
- Any patterns or anomalies in the debug logs
- Observations from the visual debugging
- Insights from interactive debugging sessions

This report will help in identifying the root cause of the issue and guide further development or bug fixing efforts.
