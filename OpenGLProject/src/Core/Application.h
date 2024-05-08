#pragma once

#include <memory>

class ImGuiManager;
class BatchRenderer;
class Renderer;
class ShaderManager;
class Level;
class Camera;
class TerrainGenerator;

struct GLFWwindow;

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
		bool& GetVSync();

        /*
         * Enable linear color space it improves the quality of the colors 
        */
        void SetLinearColorSpace( bool enable );

		/*
		 * Wireframe mode
		 */
		void SetPolygonMode();
        bool& GetPolygonMode();
	
        int GetMaxSlotForTextures() const;
	
		int m_TriangleCount;
		int m_ModelCount;
		int m_WindowWidth;
		int m_CountDraws;

    private:
    
        GLFWwindow* CreateWindow(const char* title);
        void SetCurrentLevel(Level* level);
	
        void ApplyAppIcon() const;

        void UpdateDeltaTime();

        std::unique_ptr<Level> m_currentLevel;
        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<ShaderManager> m_shaderManager;
        std::unique_ptr<Camera> m_camera;
        std::unique_ptr<BatchRenderer> m_batchRenderer;
		std::unique_ptr<TerrainGenerator> m_terrainGenerator;
		std::unique_ptr<ImGuiManager> m_imGuiManager;

        const char* m_appIcon;
	
        int m_maxSlotForTextures;
		int m_windowHeight;
	
		bool m_polygonMode;
		bool m_vSync;

		double m_lastTime;
		double m_deltaTime;

        static Application* m_instance;
};