#include "./la.h"

Vec2 vec2(float x, float y)
{
    return (Vec2) {.x = x, .y = y};
}

Vec2 vec2_add(Vec2 a, Vec2 b)
{
    return vec2(a.x + b.x, a.y + b.y);
}

Vec2 vec2_sub(Vec2 a, Vec2 b)
{
    return vec2(a.x - b.x, a.y - b.y);
}

Vec2 vec2_mul(Vec2 a, Vec2 b)
{
    return vec2(a.x * b.x, a.y * b.y);
}

Vec2 vec2_div(Vec2 a, Vec2 b)
{
    return vec2(a.x / b.x, a.y / b.y);
}

Vec2 vec2_scale(Vec2 a, float s)
{
    return vec2_mul(a, vec2(s, s));
}

