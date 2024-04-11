#pragma once

#include <cmath>

template <typename T>
struct Quaternion
{
    T w, x, y, z;
    
    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(T _w, T _x, T _y, T _z) : w(_w), x(_x), y(_y), z(_z) {}
    
    Quaternion (const Vec3<T>& eulerAngle)
    {
        Vec3<T> c = cos( eulerAngle * T(0.5) ); 
        Vec3<T> s = sin( eulerAngle * T(0.5) );

        this->w = c.x * c.y * c.z + s.x * s.y * s.z;
        this->x = s.x * c.y * c.z - c.x * s.y * s.z;
        this->y = c.x * s.y * c.z + s.x * c.y * s.z;
        this->z = c.x * c.y * s.z - s.x * s.y * c.z;
    }
    
    Vec3<T> operator*(const Vec3<T>& vec) const
    {
        Vec3<T> vn(vec);
        vn.normalize();
        
        Quaternion vecQuat(0, vn.x, vn.y, vn.z);
        Quaternion res = (*this) * vecQuat * this->conjugate();
        
        return Vec3<T>(res.x, res.y, res.z);
    }
    
    friend Vec3<T> operator*(const Vec3<T>& vec, const Quaternion& quat)
    {
        Vec3<T> vn(vec);
        vn.normalize();
        
        Quaternion vecQuat(0, vn.x, vn.y, vn.z);
        Quaternion res = quat * vecQuat * quat.conjugate();
        
        return Vec3<T>(res.x, res.y, res.z);
    }

    Quaternion operator*(const Quaternion& other) const
    {
        T nw = w * other.w - x * other.x - y * other.y - z * other.z;
        T nx = x * other.w + w * other.x + y * other.z - z * other.y;
        T ny = y * other.w + w * other.y + z * other.x - x * other.z;
        T nz = z * other.w + w * other.z + x * other.y - y * other.x;
        
        return Quaternion(nw, nx, ny, nz);
    }
    
    Quaternion conjugate() const
    {
        return Quaternion(w, -x, -y, -z);
    }
    
    Vec3<T> cos(const Vec3<T>& vec) const
    {
        return Vec3<T>(std::cos(vec.x), std::cos(vec.y), std::cos(vec.z));
    }
    
    Vec3<T> sin(const Vec3<T>& vec) const
    {
        return Vec3<T>(std::sin(vec.x), std::sin(vec.y), std::sin(vec.z));
    }
};
