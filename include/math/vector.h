#ifndef VECTOR_H
#define VECTOR_H

#define VEC_T double

typedef struct
{
    VEC_T x;
    VEC_T y;
} Vec2;

Vec2 vec2_zero();
Vec2 vec2_one();
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_sub(Vec2 a, Vec2 b);
Vec2 vec2_mul(Vec2 a, VEC_T b);
Vec2 vec2_div(Vec2 a, VEC_T b);
Vec2 vec2_neg(Vec2 a);
Vec2 vec2_norm(Vec2 a);

VEC_T vec2_dot(Vec2 a, Vec2 b);
VEC_T vec2_mag(Vec2 a);
VEC_T vec2_dist(Vec2 a, Vec2 b);

#endif // VECTOR_H