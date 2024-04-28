#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

class BatchRenderer;
class Renderer;
class ShaderManager;
class Level;
class Camera;
class TerrainGenerator;

class Application
{
    
    public:

        Application();
        virtual ~Application();

        void Run();
        void SetWindowIcon(const char* path);

        static Application* Get();
    
        const Renderer* GetRenderer() const;
        ShaderManager* GetShaderManager() const;
        Camera* GetCamera() const;
        BatchRenderer* GetBatchRenderer() const;
		TerrainGenerator* GetTerrainGenerator() const;
	
        /* 
           * Enable face culling it improves performance by not rendering the back of the faces
           * You will see performance improvement on complex models or large scenes
        */
        void SetFaceCulling( bool enable );

        /*
            * Deactivate VSync and uncap the frame rate
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
        bool& GetPolygoneMode();

        int GetMaxSlotForTextures() const;

    private:
    
        GLFWwindow* CreateWindow(const char* title, int height, int width);
        void SetCurrentLevel(Level* level);

        void DestroyImGui();
        void InitImGui(GLFWwindow* window);
        void OnImGuiRender() const;

        void ApplyAppIcon() const;

        void UpdateDeltaTime(double& lastTime, double& deltaTime);

        std::unique_ptr<Level> m_CurrentLevel;
        std::unique_ptr<Renderer> m_Renderer;
        std::unique_ptr<ShaderManager> m_ShaderManager;
        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<BatchRenderer> m_BatchRenderer;
		std::unique_ptr<TerrainGenerator> m_TerrainGenerator;

        const char* m_AppIcon;
        int m_MaxSlotForTextures;

		bool m_polygoneMode;

        static Application* m_Instance;
};