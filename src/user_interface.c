#include "user_interface.h"

#include "raylib.h"
#include "raygui.h"

void init_ui_state(UIState *state)
{
    state->current_tool = UI_TOOL_SELECT;
    state->select_mode = SELECT_MODE_BOX;
}

void draw_ui(UIState *state)
{
    int tool_button_size = 40;
    int tool_spacing = 10;
    int tool_offset = 10;
    int tool_option_button_size = 30;
    int tool_option_spacing = 5;

    // Mouse pressed down this frame
    bool mouse_clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    // Draw the select tool
    Rectangle select_tool_rect = {tool_offset, tool_offset, tool_button_size, tool_button_size};
    if (GuiButton(select_tool_rect, "Select") && mouse_clicked)
    {
        state->current_tool = UI_TOOL_SELECT;
    }

    // Draw the move tool
    Rectangle move_tool_rect = {select_tool_rect.x + tool_button_size + tool_spacing, tool_offset, tool_button_size, tool_button_size};
    if (GuiButton(move_tool_rect, "Move") && mouse_clicked)
    {
        state->current_tool = UI_TOOL_MOVE;
    }

    // Draw the spawn tool
    Rectangle spawn_tool_rect = {move_tool_rect.x + tool_button_size + tool_spacing, tool_offset, tool_button_size, tool_button_size};
    if (GuiButton(spawn_tool_rect, "Spawn") && mouse_clicked)
    {
        state->current_tool = UI_TOOL_SPAWN;
    }

    // Draw tool options
    if (state->current_tool == UI_TOOL_SELECT)
    {
        // Draw the select mode options
        Rectangle select_mode_rect = {select_tool_rect.x, select_tool_rect.y + tool_button_size + tool_spacing, tool_option_button_size, tool_option_button_size};
        if (GuiButton(select_mode_rect, "Box") && mouse_clicked)
        {
            state->select_mode = SELECT_MODE_BOX;
        }
    }
    else if (state->current_tool == UI_TOOL_MOVE)
    {
        // Draw the move tool options
        Rectangle move_mode_rect = {move_tool_rect.x, move_tool_rect.y + tool_button_size + tool_spacing, tool_option_button_size, tool_option_button_size};
        if (GuiButton(move_mode_rect, "Position") && mouse_clicked)
        {
            state->move_mode = MOVE_MODE_POSITION;
        }
    }
}