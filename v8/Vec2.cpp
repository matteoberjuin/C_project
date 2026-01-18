#pragma once
#include <cmath>

struct Vec2
{
    float x;
    float y;

    Vec2(float x_ = 0.f, float y_ = 0.f) : x(x_), y(y_) {}

    float length() const
    {
        return std::sqrt(x * x + y * y);
    }
};

// Opérateurs de base
inline Vec2 operator+(const Vec2& a, const Vec2& b) { return {a.x + b.x, a.y + b.y}; }
inline Vec2 operator-(const Vec2& a, const Vec2& b) { return {a.x - b.x, a.y - b.y}; }

inline Vec2 operator*(const Vec2& v, float s) { return {v.x * s, v.y * s}; }
inline Vec2 operator*(float s, const Vec2& v) { return {v.x * s, v.y * s}; }

inline Vec2 operator/(const Vec2& v, float s) { return {v.x / s, v.y / s}; }

// Opérateurs d’affectation
inline Vec2& operator+=(Vec2& a, const Vec2& b) { a.x += b.x; a.y += b.y; return a; }
inline Vec2& operator-=(Vec2& a, const Vec2& b) { a.x -= b.x; a.y -= b.y; return a; }
inline Vec2& operator*=(Vec2& v, float s) { v.x *= s; v.y *= s; return v; }
inline Vec2& operator/=(Vec2& v, float s) { v.x /= s; v.y /= s; return v; }
