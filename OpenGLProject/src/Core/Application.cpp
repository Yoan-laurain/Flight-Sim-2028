#include "Application.h"
#include "../Renderer/Renderer.h"
#include "../Config.h"
#include "../Levels/3D/Level3D.h"
#include "../Camera/Camera.h"
#include "../OpenGL/Shader/Shader.h"
#include "../Managers/ShaderManager/ShaderManager.h"
#include "../Managers/BatchRenderer/BatchRenderer.h"
#include "Vendor/stb_image/stb_image.h"
#include "Terrain/TerrainGenerator.h"
#include "Managers/ImGui/ImGuiManager.h"
#include <iostream>

Application* Application::m_Instance = nullptr;

Application::Application() : m_AppIcon(nullptr), m_MaxSlotForTextures(0), m_polygoneMode(false)
{
    m_Instance = this;
}

Application::~Application() = default;

void Application::UpdateDeltaTime(double& lastTime, double& deltaTime)
{
    const double currentTime = glfwGetTime();
    deltaTime =  currentTime - lastTime;
    lastTime = currentTime;
}

void Application::Run()
{
    GLFWwindow* window = CreateWindow(AppName);

    SetWindowIcon(AppIconPath);
    ApplyAppIcon();
    
    //SetFaceCulling(true); // Ne fonctionne pas avec le terrain
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_MaxSlotForTextures);

    m_Renderer = std::make_unique<Renderer>();
    m_ShaderManager = std::make_unique<ShaderManager>();
    m_BatchRenderer = std::make_unique<BatchRenderer>();
    m_TerrainGenerator = std::make_unique<TerrainGenerator>();
    m_ImGuiManager = std::make_unique<ImGuiManager>();
    m_Camera = std::make_unique<Camera>(m_WindowWidth, m_WindowHeight, Vec3(50.0f, 40.0f, 40.0f));

    m_ImGuiManager->InitImGui(window);
    
    SetCurrentLevel(new Level3D());

    SetVSync(true);

    m_CurrentLevel->BeginPlay();

    double lastTime = 0.0f;
    double deltaTime = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        UpdateDeltaTime(lastTime, deltaTime);

        m_Renderer->Clear();

        m_Camera->Update(window,deltaTime);
        m_CurrentLevel->OnRender();
        m_ImGuiManager->OnImGuiRender(m_CurrentLevel.get());
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
}

void Application::SetWindowIcon(const char* path)
{
    m_AppIcon = path;
}

Application* Application::Get()
{
    if (!m_Instance)
        m_Instance = new Application();
    
    return m_Instance;
}

const Renderer* Application::GetRenderer() const
{
    return m_Renderer.get();
}

ShaderManager* Application::GetShaderManager() const
{
    return m_ShaderManager.get();
}

Camera* Application::GetCamera() const
{
    return m_Camera.get();
}

BatchRenderer* Application::GetBatchRenderer() const
{
    return m_BatchRenderer.get();
}

TerrainGenerator* Application::GetTerrainGenerator() const
{
    return m_TerrainGenerator.get();
}

void Application::ApplyAppIcon() const
{
    if (!m_AppIcon)
        return;
    
    GLFWimage images[1];
    images[0].pixels = stbi_load(m_AppIcon, &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(glfwGetCurrentContext(), 1, images);
    stbi_image_free(images[0].pixels);
}

GLFWwindow* Application::CreateWindow(const char* title)
{
    if (!glfwInit())
        return nullptr;

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    m_WindowHeight = mode->height - 50;
    m_WindowWidth = mode->width;
    
    GLFWwindow* window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, title, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    
    glfwSetWindowPos(window, (mode->width - m_WindowWidth) / 2, (mode->height - m_WindowHeight) / 2);
    glfwSetWindowSizeLimits(window, m_WindowWidth, m_WindowHeight, m_WindowWidth, m_WindowHeight);

    if (glewInit() != GLEW_OK)
        std::cout << "Error in GLEW initialization" << std::endl;

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
    m_CurrentLevel = std::unique_ptr<Level>(level);
}

void Application::SetFaceCulling(const bool enable)
{
    if (enable)
    {
        glEnable(GL_CULL_FACE); 
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void Application::SetVSync(const bool enable)
{
    if (enable)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
}

void Application::SetLinearColorSpace(const bool enable)
{
    if (enable)
        glEnable(GL_FRAMEBUFFER_SRGB);
    else
        glDisable(GL_FRAMEBUFFER_SRGB);
}

void Application::SetPolygoneMode()
{
    if (m_polygoneMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool& Application::GetPolygoneMode()
{
	return m_polygoneMode;
}

int Application::GetMaxSlotForTextures() const
{
    return m_MaxSlotForTextures;
}