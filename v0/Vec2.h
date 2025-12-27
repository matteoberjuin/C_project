#pragma once

struct Vec2
{
    float x;
    float y;

    Vec2(float x_=0.f, float y_=0.f) : x(x_), y(y_) {}
};

inline Vec2 operator+(const Vec2& a, const Vec2& b)
{
    return {a.x + b.x, a.y + b.y};
}

inline Vec2 operator-(const Vec2& a, const Vec2& b)
{
    return {a.x - b.x, a.y - b.y};
}

inline Vec2 operator*(float s, const Vec2& a)
{
    return {s * a.x, s * a.y};
}

inline Vec2 operator*(const Vec2& a, float s)
{
    return {s * a.x, s * a.y};
}
