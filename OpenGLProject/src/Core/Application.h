#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

class BatchRenderer;
class Renderer;
class ShaderManager;
class Level;
class Camera;

class Application
{
    
    public:

        Application();
        virtual ~Application();

        void Run();
        void SetWindowIcon(const char* path);

        static Application* Get();
    
        const Renderer* GetRenderer();
        ShaderManager* GetShaderManager();
        Camera* GetCamera();
        BatchRenderer* GetBatchRenderer();

        /* 
           * Enable face culling it improves performance by not rendering the back of the faces
           * You will see performance improvement on complex models or large scenes
        */
        void SetFaceCulling( bool enable );

        /*
            * Deactivate VSync and uncap the frame rate
            * Be careful : Inputs will be based on the frame rate youll need to implement a deltaTime system
        */
        void SetVSync( bool enable );

        /*
         * Enable linear color space it improves the quality of the colors 
        */
        void SetLinearColorSpace( bool enable );

        /*
         * Enable stencil test it allows you to create a mask for your objects
         * Needed for outline shaders or post processing effects
        */
        void SetStencilTest( bool enable );

		/*
		 * Wireframe mode
		 */
		void SetPolygoneMode();
        bool* GetPolygoneMode();

        int GetMaxSlotForTextures();
		

    private:
    
        GLFWwindow* CreateWindow(const char* title, int height, int width);
        void SetCurrentLevel(Level* level);

        void DestroyImGui();
        void InitImGui(GLFWwindow* window);
        void OnImGuiRender();

        void ApplyAppIcon();

        void UpdateDeltaTime(float& lastTime, float& deltaTime);

        std::unique_ptr<Level> m_CurrentLevel;
        std::unique_ptr<Renderer> m_Renderer;
        std::unique_ptr<ShaderManager> m_ShaderManager;
        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<BatchRenderer> m_BatchRenderer;


        const char* m_AppIcon;
        int MaxSlotForTextures;

		bool m_polygoneMode; // Wireframe mode

        static Application* m_Instance;
};