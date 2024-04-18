#pragma once

#include "Library/Vec3.h"
#include <functional>

namespace MyImGui
{
    bool SliderFloat3(const char* label,const Vec3<float>& position, float minVal, float maxVal, const std::function<void(const Vec3<float>&)>& callback);
}
