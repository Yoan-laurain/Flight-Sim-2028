#pragma once

#include <cmath>

template <typename T>
struct Vec3
{
    T x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    Vec3(T _value) : x(_value), y(_value), z(_value) {}

    Vec3 operator+(const Vec3& other) const
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    
    Vec3 operator-(const Vec3& other) const
    {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    
    Vec3 operator*(T scalar) const
    {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    
    friend Vec3 operator*= (Vec3& vec, const float scalar)
    {
        vec.x *= scalar;
        vec.y *= scalar;
        vec.z *= scalar;
        return vec;
    }
    
    friend Vec3 operator*(const float scalar, const Vec3& vec)
    {
        return Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
    }
    
    T& operator[](int index)
    {
        return (&x)[index];
    }
    
    Vec3 operator+(const float val) const
    {
        return Vec3(x + val, y + val, z + val);
    }
    
    static Vec3<T> Up()
    {
        return Vec3<T>(0, 1, 0);
    }
    
    Vec3& operator+=(const Vec3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    Vec3& operator-()
    {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }
    
    Vec3 operator/(const float val) const
    {
        return Vec3(x / val, y / val, z / val);
    }
    
    Vec3 cross(const Vec3& other) const
    {
        return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }
    
    Vec3 normalize() const
    {
        T mag = magnitude();
        return Vec3(x / mag, y / mag, z / mag);
    }
    
    T dot(const Vec3& vec2)
    {
        return x * vec2.x + y * vec2.y + z * vec2.z;
    }
    
    T magnitude() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3& operator=(const Vec3& other)
    {
        if (this != &other) {
            this->x = other.x;
            this->y = other.y;
            this->z = other.z;
        }
        return *this;
    }
};
