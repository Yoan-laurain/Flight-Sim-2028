#pragma once

#include <GLFW/glfw3.h>

class Level;

class ImGuiManager
{
    public:
        ImGuiManager();
        ~ImGuiManager();
    
        void InitImGui(GLFWwindow* window);
        void OnImGuiRender( Level* currentLevel ) const;

    private:
        void DestroyImGui();
};
