#pragma once

#include "Mat3.h"
#include <cstring>

#include "Vec3.h"
#include "Vec4.h"

template <typename T>
struct Mat4
{
    T m[4][4];
    
    Mat4() {
        memset(m, 0, sizeof(m));
    }

    Mat4(T _value) {
        memset(m, 0, sizeof(m));
        for (int i = 0; i < 4; ++i) {
            m[i][i] = _value;
        }
    }

    Mat4 (const Mat3<T>& mat3) {
        memset(m, 0, sizeof(m));
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                m[i][j] = mat3[i][j];
            }
        }
        m[3][3] = static_cast<T>(0);
    }
    
    void SetToIdentity() {
        memset(m, 0, sizeof(m));
        for (int i = 0; i < 4; ++i) {
            m[i][i] = static_cast<T>(1);
        }
    }
    
    T* operator[](int index) {
        return m[index];
    }
    
    const T* operator[](int index) const {
        return m[index];
    }
    
    Vec4<T> GetRow(int row) const
    {
        return Vec4<T>(m[row][0], m[row][1], m[row][2], m[row][3]);
    }

    // Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2] + SrcA3 * SrcB0[3];
    // Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2] + SrcA3 * SrcB1[3];
    // Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2] + SrcA3 * SrcB2[3];
    // Result[3] = SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2] + SrcA3 * SrcB3[3];
    
    Mat4 operator*(const Mat4& other) const
    {
        auto SrcA0 = GetRow(0);
        auto SrcA1 = GetRow(1);
        auto SrcA2 = GetRow(2);
        auto SrcA3 = GetRow(3);

        auto SrcB0 = other.GetRow(0);
        auto SrcB1 = other.GetRow(1);
        auto SrcB2 = other.GetRow(2);
        auto SrcB3 = other.GetRow(3);

        Mat4<T> Result;
        Result.SetRow(0, Vec4<T>(SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2] + SrcA3 * SrcB0[3]));
        Result.SetRow(1, Vec4<T>(SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2] + SrcA3 * SrcB1[3]));
        Result.SetRow(2, Vec4<T>(SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2] + SrcA3 * SrcB2[3]));
        Result.SetRow(3, Vec4<T>(SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2] + SrcA3 * SrcB3[3]));
        
        return Result;
    }
    
    Mat4& operator=(const Mat4& mat4)
    {
        if (this != &mat4) { 
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    this->m[i][j] = mat4.m[i][j];
                }
            }
        }
        return *this;
    }

    Mat4& SetRow(int row, Vec4<T> vec)
    {
        m[row][0] = vec.x;
        m[row][1] = vec.y;
        m[row][2] = vec.z;
        m[row][3] = vec.w;
        return *this;
    }
};
