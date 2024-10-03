#include "ui_handler.h"
#include "unity.h"
#include "raylib.h"

void setUp(void) {}
void tearDown(void) {}

void test_GetScreenToWorld2D(void) {
    Camera2D camera = {0};
    camera.target = (Vector2){0, 0};
    camera.offset = (Vector2){400, 300};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Vector2 screenPos = {400, 300};
    Vector2 worldPos = GetScreenToWorld2D(screenPos, camera);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, worldPos.x);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, worldPos.y);
}

void test_GetWorldToScreen2D(void) {
    Camera2D camera = {0};
    camera.target = (Vector2){0, 0};
    camera.offset = (Vector2){400, 300};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Vector2 worldPos = {0, 0};
    Vector2 screenPos = GetWorldToScreen2D(worldPos, camera);

    TEST_ASSERT_EQUAL_FLOAT(400.0f, screenPos.x);
    TEST_ASSERT_EQUAL_FLOAT(300.0f, screenPos.y);
}

void test_add_to_selection(void) {
    UIState state;
    init_ui_state(&state);

    add_to_selection(&state, 1);
    TEST_ASSERT_EQUAL_INT(1, state.selected_count);
    TEST_ASSERT_EQUAL_INT(1, state.selected_particles[0]);

    deinit_ui_state(&state);
}

void test_remove_from_selection(void) {
    UIState state;
    init_ui_state(&state);

    add_to_selection(&state, 1);
    remove_from_selection(&state, 1);
    TEST_ASSERT_EQUAL_INT(0, state.selected_count);

    deinit_ui_state(&state);
}

void test_toggle_particle_selection(void) {
    UIState state;
    init_ui_state(&state);

    toggle_particle_selection(&state, 1);
    TEST_ASSERT_EQUAL_INT(1, state.selected_count);
    TEST_ASSERT_EQUAL_INT(1, state.selected_particles[0]);

    toggle_particle_selection(&state, 1);
    TEST_ASSERT_EQUAL_INT(0, state.selected_count);

    deinit_ui_state(&state);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_GetScreenToWorld2D);
    RUN_TEST(test_GetWorldToScreen2D);
    RUN_TEST(test_add_to_selection);
    RUN_TEST(test_remove_from_selection);
    RUN_TEST(test_toggle_particle_selection);
    return UNITY_END();
}
