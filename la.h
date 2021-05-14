#ifndef LA_H_
#define LA_H_

typedef struct {
    float x, y;
} Vec2;

Vec2 vec2(float x, float y);
Vec2 vec2s(float x);
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_sub(Vec2 a, Vec2 b);
Vec2 vec2_mul(Vec2 a, Vec2 b);
Vec2 vec2_div(Vec2 a, Vec2 b);
Vec2 vec2_pow(Vec2 a, Vec2 b);

#endif // LA_H_
