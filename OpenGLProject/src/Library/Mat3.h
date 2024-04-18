#pragma once

#include "Vec3.h"
#include <cstring>

template <typename T>
struct Mat3
{
    T m[3][3];
    
    Mat3() {
        memset(m, 0, sizeof(m));
    }
    
    Mat3(T _value) {
        memset(m, 0, sizeof(m));
        for (int i = 0; i < 3; ++i) {
            m[i][i] = _value;
        }
    }
    
    void SetToIdentity() {
        memset(m, 0, sizeof(m));
        for (int i = 0; i < 3; ++i) {
            m[i][i] = static_cast<T>(1);
        }
    }
    
    T* operator[](int index) {
        return m[index];
    }
    
    const T* operator[](int index) const {
        return m[index];
    }
    
    Mat3 operator*(T _value) const {
        Mat3 result;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result[i][j] = m[i][j] * _value;
            }
        }
        return result;
    }
    
    Vec3<T> operator*(Vec3<T>& _v)  {
        Vec3<T> result;
        for (int i = 0; i < 3; ++i) {
            result[i] = m[i][0] * _v[0] + m[i][1] * _v[1] + m[i][2] * _v[2];
        }
        return result;
    }
};
