#include <math.h>
#include "./la.h"

Vec2 vec2(float x, float y)
{
    return (Vec2) {.x = x, .y = y};
}

Vec2 vec2_from_ivec2(iVec2 a)
{
    return vec2((float) a.x, (float) a.y);
}

Vec2 vec2s(float x)
{
    return vec2(x, x);
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

Vec2 vec2_pow(Vec2 a, Vec2 b)
{
    return vec2(powf(a.x, b.x), powf(a.y, b.y));
}

Vec2 vec2_clamp(Vec2 a, Vec2 l, Vec2 h)
{
    if (a.x < l.x) a.x = l.x;
    if (a.x > h.x) a.x = h.x;
    if (a.y < l.y) a.y = l.y;
    if (a.y > h.y) a.y = h.y;
    return a;
}

//////////////////////////////

iVec2 ivec2(int x, int y)
{
    return (iVec2) {.x = x, .y = y};
}

iVec2 ivec2_from_vec2(Vec2 a)
{
    return ivec2((int) a.x, (int) a.y);
}

iVec2 ivec2s(int x)
{
    return ivec2(x, x);
}

iVec2 ivec2_add(iVec2 a, iVec2 b)
{
    return ivec2(a.x + b.x, a.y + b.y);
}

iVec2 ivec2_sub(iVec2 a, iVec2 b)
{
    return ivec2(a.x - b.x, a.y - b.y);
}

iVec2 ivec2_mul(iVec2 a, iVec2 b)
{
    return ivec2(a.x * b.x, a.y * b.y);
}

iVec2 ivec2_div(iVec2 a, iVec2 b)
{
    return ivec2(a.x / b.x, a.y / b.y);
}
