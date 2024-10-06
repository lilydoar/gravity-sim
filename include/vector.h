#ifndef VECTOR_H
#define VECTOR_H

#define VECTOR_T double

typedef struct
{
    VECTOR_T x;
    VECTOR_T y;
} Vec2;

Vec2 vec2_zero();
Vec2 vec2_one();
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_sub(Vec2 a, Vec2 b);
Vec2 vec2_mul(Vec2 a, Vec2 b);
Vec2 vec2_div(Vec2 a, Vec2 b);
Vec2 vec2_scale(Vec2 a, VECTOR_T s);
Vec2 vec2_neg(Vec2 a);
Vec2 vec2_norm(Vec2 a);

VECTOR_T vec2_dot(Vec2 a, Vec2 b);
VECTOR_T vec2_len(Vec2 a);
VECTOR_T vec2_dist(Vec2 a, Vec2 b);
Vec2 vec2_lerp(Vec2 a, Vec2 b, VECTOR_T t);

#endif // VECTOR_H