#ifndef UI_HANDLER_H
#define UI_HANDLER_H

#include <cglm/struct.h>
#include <arena_allocator.h>

/*
UI

Square buttons along the top of the screen for tools. Icons for each button.
Hover for tooltips.

Tool options appear as smaller boxes under each tool.
Only draw the options of the selected tool.

Click to set action: select, move, spawn, etc...
1. Select
- Arrow pointer icon
- Single click: select single particle
- Click and drag: box select
- Shift + click: add to selection
- Ctrl + click: remove from selection
- Double click: select all particles in the arena
- rectangle select mode
- circle select mode

2. Move
- crossed arrows icon
- Click and drag: modify selected particles
- position mode: set position of selected particles
- velocity mode: set velocity of selected particles
- acceleration mode: set acceleration of selected particles
- For vel and acc mode clicking and dragging will draw an arrow indicating the
  force vector being applied to the selected particles

3. Spawn
- plus icon
- Tint red if particle spawn collides with other particles, green if not
a. click: particle info appears: size, mass, etc
b. drag: set particle radius
c. release: spawn particle with radius and corresponding mass
*/

typedef enum {
    UI_TOOL_SELECT,
    UI_TOOL_MOVE,
    UI_TOOL_SPAWN,
} UITool;

typedef enum {
    SELECT_MODE_BOX,
    SELECT_MODE_CIRCLE,
} SelectMode;

typedef struct {
    SelectMode mode;
    bool is_selecting;
    vec2s start_pos;
    vec2s current_pos;
} SelectToolsOptions;

typedef enum {
    MOVE_MODE_POSITION,
    MOVE_MODE_VELOCITY,
    MOVE_MODE_ACCELERATION,
} MoveMode;

typedef struct {
    MoveMode mode;
    bool is_moving;
    vec2s start_pos;
    vec2s current_pos;
} MoveToolsOptions;

typedef enum {
    SPAWN_MODE_SINGLE,
} SpawnMode;

typedef struct {
    SpawnMode mode;
    vec2s position;
    double radius;
} SpawnToolsOptions;

typedef struct {
    ArenaAllocator *tool_arena;
    ArenaAllocator *frame_arena;
    UITool current_tool;
    union {
        SelectToolsOptions select;
        MoveToolsOptions move;
        SpawnToolsOptions spawn;
    } options;
} UIState;

void init_ui_state(UIState *state, ArenaAllocator *tool_arena, ArenaAllocator *frame_arena);
void deinit_ui_state(UIState *state);

void handle_input(UIState *state);
bool draw_ui(UIState *state);

#endif // UI_HANDLER_H