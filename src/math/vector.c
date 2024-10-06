#include "math/vector.h"

#include <math.h>

Vec2 vec2_zero()
{
    return (Vec2){0.0, 0.0};
}
Vec2 vec2_one()
{
    return (Vec2){1.0, 1.0};
}
Vec2 vec2_add(Vec2 a, Vec2 b)
{
    return (Vec2){
        a.x + b.x,
        a.y + b.y};
}
Vec2 vec2_sub(Vec2 a, Vec2 b)
{
    return (Vec2){
        a.x - b.x,
        a.y - b.y};
}
Vec2 vec2_mul(Vec2 a, VEC_T b)
{
    return (Vec2){
        a.x * b,
        a.y * b};
}
Vec2 vec2_div(Vec2 a, VEC_T b)
{
    return (Vec2){
        a.x / b,
        a.y / b};
}
Vec2 vec2_neg(Vec2 a)
{
    return (Vec2){
        -a.x,
        -a.y};
}
Vec2 vec2_norm(Vec2 a)
{
    VEC_T mag = vec2_mag(a);
    return vec2_div(a, mag);
}

VEC_T vec2_dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}
VEC_T vec2_mag(Vec2 a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}
VEC_T vec2_dist(Vec2 a, Vec2 b)
{
    return vec2_mag(vec2_sub(a, b));
}