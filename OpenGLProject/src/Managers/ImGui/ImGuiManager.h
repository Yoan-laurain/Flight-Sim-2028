#pragma once

struct GLFWwindow;
class Level;

class ImGuiManager
{
    public:
        ImGuiManager();
        ~ImGuiManager();
    
        void InitImGui(GLFWwindow* window);
        void OnImGuiRender(Level* currentLevel) const;

    private:
        void DestroyImGui();
};
