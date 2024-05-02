#include "Application.h"
#include "Renderer/Renderer.h"
#include "Config.h"
#include "Camera/Camera.h"
#include "OpenGL/Shader/Shader.h"
#include "Managers/ShaderManager/ShaderManager.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include "Vendor/stb_image/stb_image.h"
#include "Terrain/TerrainGenerator.h"
#include "Managers/ImGui/ImGuiManager.h"
#include "Levels/ProceduralWorld/ProceduralWorld.h"
#include <iostream>
#include <GLFW/glfw3.h>

Application* Application::m_instance = nullptr;

Application::Application()
    : m_TriangleCount(0)
    , m_ModelCount(0)
    , m_WindowWidth(0)
    , m_CountDraws(0)
    , m_appIcon(nullptr)
    , m_maxSlotForTextures(0)
    , m_windowHeight(0)
    , m_polygonMode(false)
    , m_vSync(true)
    , m_lastTime(0.0)
    , m_deltaTime(0.0)
{
    m_instance = this;
}

Application::~Application() = default;

void Application::Run()
{
    GLFWwindow* window = CreateWindow(AppName);

    SetWindowIcon(AppIconPath);
    ApplyAppIcon();
    
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxSlotForTextures);

    m_renderer = std::make_unique<Renderer>();
    m_shaderManager = std::make_unique<ShaderManager>();
    m_batchRenderer = std::make_unique<BatchRenderer>();
    m_terrainGenerator = std::make_unique<TerrainGenerator>();
    m_imGuiManager = std::make_unique<ImGuiManager>();
    m_camera = std::make_unique<Camera>(m_WindowWidth, m_windowHeight, Vec3(50.0f, 40.0f, 40.0f));

    m_imGuiManager->InitImGui(window);
    
    SetCurrentLevel(new ProceduralWorld());

    SetVSync(m_vSync);

    m_currentLevel->BeginPlay();

    while (!glfwWindowShouldClose(window))
    {
        UpdateDeltaTime();

        m_renderer->Clear();

        m_camera->Update(window,m_deltaTime);
        m_currentLevel->OnRender();
        m_imGuiManager->OnImGuiRender(m_currentLevel.get());
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
}

void Application::UpdateDeltaTime()
{
    const double currentTime = glfwGetTime();
    m_deltaTime =  currentTime - m_lastTime;
    m_lastTime = currentTime;
}

void Application::SetWindowIcon(const char* path)
{
    m_appIcon = path;
}

Application* Application::Get()
{
    if (!m_instance)
    {
        m_instance = new Application();
    }
    return m_instance;
}

const Renderer* Application::GetRenderer() const
{
    return m_renderer.get();
}

ShaderManager* Application::GetShaderManager() const
{
    return m_shaderManager.get();
}

Camera* Application::GetCamera() const
{
    return m_camera.get();
}

BatchRenderer* Application::GetBatchRenderer() const
{
    return m_batchRenderer.get();
}

TerrainGenerator* Application::GetTerrainGenerator() const
{
    return m_terrainGenerator.get();
}

void Application::ApplyAppIcon() const
{
    if (!m_appIcon)
    {
        return;
    }
    
    GLFWimage images[1];
    images[0].pixels = stbi_load(m_appIcon, &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(glfwGetCurrentContext(), 1, images);
    stbi_image_free(images[0].pixels);
}

GLFWwindow* Application::CreateWindow(const char* title)
{
    if (!glfwInit())
    {
        return nullptr;
    }

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    m_windowHeight = mode->height - 50;
    m_WindowWidth = mode->width;
    
    GLFWwindow* window = glfwCreateWindow(m_WindowWidth, m_windowHeight, title, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, m_WindowWidth, m_windowHeight);
    
    glfwSetWindowPos(window, (mode->width - m_WindowWidth) / 2, (mode->height - m_windowHeight) / 2);
    glfwSetWindowSizeLimits(window, m_WindowWidth, m_windowHeight, m_WindowWidth, m_windowHeight);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error in GLEW initialization" << std::endl;
    }

    glDebugMessageCallback(Renderer::HandleError, nullptr);

    /*
    * By putting the next two lines, we are able to use the callstack to go back to the line where the error occured if you put a breakpoint in the HandleError function
    */

    // If enabled, debug messages are produced by a debug context
    glEnable(GL_DEBUG_OUTPUT); 

    // If enabled, debug messages are produced synchronously by a debug context
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return window;
}

void Application::SetCurrentLevel(Level* level)
{
    m_currentLevel = std::unique_ptr<Level>(level);
}

void Application::SetFaceCulling(const bool enable)
{
    if (enable)
    {
        glEnable(GL_CULL_FACE); 
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        return;
    }
    glDisable(GL_CULL_FACE);
}

void Application::SetVSync(const bool enable)
{
    glfwSwapInterval(enable);
}

bool& Application::GetVSync()
{
    return m_vSync;
}

void Application::SetLinearColorSpace(const bool enable)
{
    if (enable)
    {
        glEnable(GL_FRAMEBUFFER_SRGB);
        return;
    }
    glDisable(GL_FRAMEBUFFER_SRGB);
}

void Application::SetPolygonMode()
{
    glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode?GL_LINE:GL_FILL);
}

bool& Application::GetPolygonMode()
{
	return m_polygonMode;
}

int Application::GetMaxSlotForTextures() const
{
    return m_maxSlotForTextures;
}