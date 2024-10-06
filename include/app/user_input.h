#ifndef USER_INPUT_H
#define USER_INPUT_H

#include "raylib.h"

#include <stdbool.h>

typedef struct {
    bool mouse_left_pressed;
    bool mouse_left_released;
    Vector2 mouse_start;
    Vector2 mouse_current;
} UserInput;

// Collect input from the user
void collect_input(UserInput *input);

#endif // USER_INPUT_H