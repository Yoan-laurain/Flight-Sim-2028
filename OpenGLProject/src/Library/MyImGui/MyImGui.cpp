#include "MyImGui.h"
#include <imgui.h>

bool MyImGui::SliderFloat3(const Vec3<float>& position, const float minVal, const float maxVal, const std::function<void(const Vec3<float>&)>& callback)
{
    Vec3<float> newPosition = position;
	
    bool valueChanged = false;
    valueChanged |= ImGui::SliderFloat("X", &newPosition.x, minVal, maxVal); 
    valueChanged |= ImGui::SliderFloat("Y", &newPosition.y, minVal, maxVal);
    valueChanged |= ImGui::SliderFloat("Z", &newPosition.z, minVal, maxVal);
	
    if (valueChanged)
    {
        callback(newPosition); 
    }

    return valueChanged;
}