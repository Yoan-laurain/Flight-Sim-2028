#include "MyImGui.h"

#include <format>
#include <imgui.h>

bool MyImGui::SliderFloat3(const char* label,const Vec3<float>& position, const float minVal, const float maxVal, const std::function<void(const Vec3<float>&)>& callback)
{
    Vec3<float> newPosition = position;
	
    bool valueChanged = false;
    valueChanged |= ImGui::SliderFloat( std::format("{} X", label).c_str(), &newPosition.x, minVal, maxVal);
    valueChanged |= ImGui::SliderFloat( std::format("{} Y", label).c_str(), &newPosition.y, minVal, maxVal);
    valueChanged |= ImGui::SliderFloat( std::format("{} Z", label).c_str(), &newPosition.z, minVal, maxVal);
	
    if (valueChanged)
    {
        callback(newPosition); 
    }

    return valueChanged;
}