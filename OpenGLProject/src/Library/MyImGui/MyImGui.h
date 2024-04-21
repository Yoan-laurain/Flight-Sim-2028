#pragma once

#include "Library/Vec3.h"
#include <functional>

namespace MyImGui
{
    bool SliderFloat3(const char* label,const Vec3<float>& position, float minVal, float maxVal, const std::function<void(const Vec3<float>&)>& callback);
    bool SliderFloat(const char* label, const float& value, float minVal, float maxVal, const std::function<void(float)>& callback);
    bool SliderInt(const char* label, const int& value, int minVal, int maxVal, const std::function<void(int)>& callback);
}
