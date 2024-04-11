#pragma once

template <typename T>
struct Vec4
{
    T x, y, z, w;

    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vec4(T _value) : x(_value), y(_value), z(_value), w(_value) {}

    Vec4 operator+(const Vec4& other) const
    {
        return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
    }
    
    Vec4 operator-(const Vec4& other) const
    {
        return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
    }
    
    Vec4 operator*(T scalar) const
    {
        return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
    }
    
    T& operator[](int index)
    {
        return (&x)[index];
    }
};