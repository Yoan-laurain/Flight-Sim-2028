#pragma once

template <typename T>
struct Vec2
{
    T x, y;

    Vec2() : x(0), y(0) {}
    Vec2(T _x, T _y) : x(_x), y(_y) {}
    Vec2(T _value ) : x(_value), y(_value) { }
    
    Vec2 operator-(const Vec2& other) const
    {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2 operator+(const Vec2<int>& other) const
    {
        return Vec2(x + other.x, y + other.y);
    }
    
    Vec2 operator*(T scalar) const
    {
        return Vec2(x * scalar, y * scalar);
    }

};