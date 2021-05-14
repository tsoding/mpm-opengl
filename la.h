#ifndef LA_H_
#define LA_H_

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    int x, y;
} iVec2;

Vec2 vec2(float x, float y);
Vec2 vec2_from_ivec2(iVec2 a);
Vec2 vec2s(float x);
Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_sub(Vec2 a, Vec2 b);
Vec2 vec2_mul(Vec2 a, Vec2 b);
Vec2 vec2_div(Vec2 a, Vec2 b);
Vec2 vec2_pow(Vec2 a, Vec2 b);
Vec2 vec2_clamp(Vec2 a, Vec2 l, Vec2 h);

iVec2 ivec2(int x, int y);
iVec2 ivec2_from_vec2(Vec2 a);
iVec2 ivec2s(int x);
iVec2 ivec2_add(iVec2 a, iVec2 b);
iVec2 ivec2_sub(iVec2 a, iVec2 b);
iVec2 ivec2_mul(iVec2 a, iVec2 b);
iVec2 ivec2_div(iVec2 a, iVec2 b);

#endif // LA_H_
