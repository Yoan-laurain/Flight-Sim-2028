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

bool MyImGui::SliderFloat(const char* label, const float& value, float minVal, float maxVal,
    const std::function<void(float)>& callback)
{
    float newValue = value;
    
    bool valueChanged = ImGui::SliderFloat(label, &newValue, minVal, maxVal);
    if (valueChanged)
    {
        callback(newValue);
    }

    return valueChanged;
}


bool MyImGui::SliderInt(const char* label, const int& value, int minVal, int maxVal,
    const std::function<void(int)>& callback)
{
    int newValue = value;
    
    bool valueChanged = ImGui::SliderInt(label, &newValue, minVal, maxVal);
    if (valueChanged)
    {
        callback(newValue);
    }

    return valueChanged;
}
