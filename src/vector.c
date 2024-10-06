#include "vector.h"

#include <math.h>

Vec2 vec2_zero() {
    return (Vec2){0, 0};
}
Vec2 vec2_one() {
    return (Vec2){1, 1};
}
Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2){a.x + b.x, a.y + b.y};
}
Vec2 vec2_sub(Vec2 a, Vec2 b) {
    return (Vec2){a.x - b.x, a.y - b.y};
}
Vec2 vec2_mul(Vec2 a, Vec2 b) {
    return (Vec2){a.x * b.x, a.y * b.y};
}
Vec2 vec2_div(Vec2 a, Vec2 b) {
    return (Vec2){a.x / b.x, a.y / b.y};
}
Vec2 vec2_scale(Vec2 a, VECTOR_T s) {
    return (Vec2){a.x * s, a.y * s};
}
Vec2 vec2_neg(Vec2 a) {
    return (Vec2){-a.x, -a.y};
}
Vec2 vec2_norm(Vec2 a) {
    VECTOR_T len = vec2_len(a);
    return (Vec2){a.x / len, a.y / len};
}

VECTOR_T vec2_dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}
VECTOR_T vec2_len(Vec2 a) {
    return sqrt(a.x * a.x + a.y * a.y);
}
VECTOR_T vec2_dist(Vec2 a, Vec2 b) {
    return vec2_len(vec2_sub(a, b));
}
Vec2 vec2_lerp(Vec2 a, Vec2 b, VECTOR_T t) {
    return vec2_add(a, vec2_scale(vec2_sub(b, a), t));
}