#pragma once

#include "Vec3.h"
#include "Mat4.h"
#include "Quaternion.h"

#include <cmath>
#include <random>

constexpr double M_PI = 3.14159265358979323846;

namespace Math
{
    // convert mat4 to mat 3
    template<typename T>
    Mat3<T> mat3_cast(const Mat4<T>& mat4)
    {
        Mat3<T> result;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result[i][j] = mat4[i][j];
            }
        }
        return result;
    }
    
    template<typename T = float>
    constexpr T radians(T degrees) {
        return degrees * M_PI / 180.f;
    }

    template<typename T>
    constexpr T cross(const T& vec1, const T& vec2)
    {
        return vec1.cross(vec2);
    }
    
    template<typename T>
    T rand(T x, T y)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<T> dis(x, y);
        return dis(gen);
    }

    inline Vec3<float> Normalize(const Vec3<float>& vec)
    {
        return vec.normalize();
    }

    static float InverseLerp(float a, float b, float value)
    {
        if (a != b)
            return (value - a) / (b - a);
        
        return 0.0f;
    }
    
    template<typename T>
    Vec3<T> rotate( Vec3<T>& v, T angle, Vec3<T>& normal)
    {
        Mat3<T> m3(1);
        Mat4<T> m(m3);
        
        T const a = angle;
        T const c = cos(a);
        T const s = sin(a);

        Vec3<T> axisNormalized = normal.normalize();
        Vec3<T> temp((T(1) - c) * axisNormalized);

        Mat4<T> Rotate;
        Rotate[0][0] = c + temp.x * axisNormalized.x;
        Rotate[0][1] = temp.x * axisNormalized.y + s * axisNormalized.z;
        Rotate[0][2] = temp.x * axisNormalized.z - s * axisNormalized.y;

        Rotate[1][0] = temp.y * axisNormalized.x - s * axisNormalized.z;
        Rotate[1][1] = c + temp.y * axisNormalized.y;
        Rotate[1][2] = temp.y * axisNormalized.z + s * axisNormalized.x;

        Rotate[2][0] = temp.z * axisNormalized.x + s * axisNormalized.y;
        Rotate[2][1] = temp.z * axisNormalized.y - s * axisNormalized.x;
        Rotate[2][2] = c + temp.z * axisNormalized.z;

        Mat4<T> Result;
        Result.SetRow(0, m.GetRow(0) * Rotate[0][0] + m.GetRow(1) * Rotate[0][1] + m.GetRow(2) * Rotate[0][2]);
        Result.SetRow(1, m.GetRow(0) * Rotate[1][0] + m.GetRow(1) * Rotate[1][1] + m.GetRow(2) * Rotate[1][2]);
        Result.SetRow(2, m.GetRow(0) * Rotate[2][0] + m.GetRow(1) * Rotate[2][1] + m.GetRow(2) * Rotate[2][2]);
        Result.SetRow(3, m.GetRow(3));
        
        Mat3<T> Result3 = mat3_cast(Result);
        Vec3<T> Res = Result3 * v;
        
        return Res;
    }

    inline Mat4<float> make_mat4(const float* values)
    {
        Mat4<float> result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[i][j] = values[i * 4 + j];
            }
        }
        return result;
    }

    template<typename T>
    Quaternion<T> make_quaternion(T* values)
    {
        return Quaternion<T>(values[0], values[1], values[2], values[3]);
    }

    template<typename T>
    Vec3<T> make_vec3(T* values)
    {
        return Vec3<T>(values[0], values[1], values[2]);
    }
    
    template<typename T>
    Mat4<T> perspective(const float fov, const float ratio, const float nearPlane, const float farPlane)
    {
        Mat4<T> result;
        float tanHalfFov = std::tan(fov / 2);
        result[0][0] = 1 / (ratio * tanHalfFov);
        result[1][1] = 1 / tanHalfFov;
        result[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result[2][3] = -1;
        result[3][2] = -(2 * farPlane * nearPlane) / (farPlane - nearPlane);
        return result;
    }

    template<typename T = float>
    Mat3<T> lookAt(const Vec3<T>& eye, const Vec3<T>& target, const Vec3<T>& up)
    {
        Vec3<T> zaxis = (eye - target).normalize();
        Vec3<T> xaxis = up.cross(zaxis).normalize();
        Vec3<T> yaxis = zaxis.cross(xaxis);

        Mat3<T> viewMatrix;
        viewMatrix[0][0] = xaxis.x;
        viewMatrix[1][0] = xaxis.y;
        viewMatrix[2][0] = xaxis.z;
        viewMatrix[0][1] = yaxis.x;
        viewMatrix[1][1] = yaxis.y;
        viewMatrix[2][1] = yaxis.z;
        viewMatrix[0][2] = zaxis.x;
        viewMatrix[1][2] = zaxis.y;
        viewMatrix[2][2] = zaxis.z;

        return viewMatrix;
    }

    template<typename T = float>
    Mat4<T> mat4_cast(const Quaternion<T>& quat)
    {
        Mat4<T> result;
        T x = quat.x;
        T y = quat.y;
        T z = quat.z;
        T w = quat.w;

        T x2 = x + x;
        T y2 = y + y;
        T z2 = z + z;

        T xx = x * x2;
        T xy = x * y2;
        T xz = x * z2;
        T yy = y * y2;
        T yz = y * z2;
        T zz = z * z2;
        T wx = w * x2;
        T wy = w * y2;
        T wz = w * z2;

        result[0][0] = 1 - (yy + zz);
        result[0][1] = xy + wz;
        result[0][2] = xz - wy;
        result[0][3] = 0;

        result[1][0] = xy - wz;
        result[1][1] = 1 - (xx + zz);
        result[1][2] = yz + wx;
        result[1][3] = 0;

        result[2][0] = xz + wy;
        result[2][1] = yz - wx;
        result[2][2] = 1 - (xx + yy);
        result[2][3] = 0;

        result[3][0] = 0;
        result[3][1] = 0;
        result[3][2] = 0;
        result[3][3] = 1;

        return result;
    }

    template<typename T = float>
    Mat4<T> Scale(const Mat4<T>& matrix, const Vec3<T>& scaling) {
        Mat4<T> result = matrix;

        result[0][0] *= scaling.x;
        result[1][1] *= scaling.y;
        result[2][2] *= scaling.z;

        return result;
    }

    template<typename T = float>
    Mat4<T> translate(const Mat4<T>& matrix, const Vec3<T>& translation) {
        Mat4<T> result = matrix;

        result[3][0] += translation.x;
        result[3][1] += translation.y;
        result[3][2] += translation.z;

        return result;
    }
    
    template<typename T = float>
    Mat4<T> lookAtM4(const Vec3<T>& eye, const Vec3<T>& center, const Vec3<T>& up)
    {
        Vec3<T> f = (center - eye).normalize();
        Vec3<T> s = cross(f, up).normalize();
        Vec3<T> u = cross(s, f);

        Mat4<T> Result(1);
        Result[0][0] = s.x;
        Result[1][0] = s.y;
        Result[2][0] = s.z;
        Result[0][1] = u.x;
        Result[1][1] = u.y;
        Result[2][1] = u.z;
        Result[0][2] = -f.x;
        Result[1][2] = -f.y;
        Result[2][2] = -f.z;
        Result[3][0] = -s.dot(eye);
        Result[3][1] = -u.dot(eye);
        Result[3][2] =  f.dot(eye);
        return Result;
    }

}