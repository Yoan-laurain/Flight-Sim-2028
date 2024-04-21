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

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

Application* Application::m_Instance = nullptr;

Application::Application() : m_AppIcon(nullptr), m_polygoneMode(false)
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
    GLFWwindow* window = CreateWindow(AppName, WindowHeight, WindowWidth);

    SetWindowIcon(AppIconPath);
    ApplyAppIcon();

    InitImGui(window);
    
    SetFaceCulling(true);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxSlotForTextures);

    m_Renderer = std::make_unique<Renderer>();
    m_ShaderManager = std::make_unique<ShaderManager>();
    m_BatchRenderer = std::make_unique<BatchRenderer>();
    m_TerrainGenerator = std::make_unique<TerrainGenerator>();
    m_Camera = std::make_unique<Camera>(WindowWidth, WindowHeight, Vec3(0.0f, 0.0f, 100.0f));

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
        
        OnImGuiRender();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    DestroyImGui();
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

const Renderer* Application::GetRenderer()
{
    return m_Renderer.get();
}

ShaderManager* Application::GetShaderManager()
{
    return m_ShaderManager.get();
}

Camera* Application::GetCamera()
{
    return m_Camera.get();
}

BatchRenderer* Application::GetBatchRenderer()
{
    return m_BatchRenderer.get();
}

TerrainGenerator* Application::GetTerrainGenerator()
{
    return m_TerrainGenerator.get();
}

void Application::OnImGuiRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Settings");
    m_CurrentLevel->OnImGuiRender();
    ImGui::End();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::ApplyAppIcon()
{
    if (!m_AppIcon)
        return;
    
    GLFWimage images[1];
    images[0].pixels = stbi_load(m_AppIcon, &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(glfwGetCurrentContext(), 1, images);
    stbi_image_free(images[0].pixels);
}

void Application::InitImGui(GLFWwindow* window)
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 440");
}

void Application::DestroyImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

GLFWwindow* Application::CreateWindow(const char* title, const int height, const int width)
{
    if (!glfwInit())
        return nullptr;

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);

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

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

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

void Application::SetStencilTest(const bool enable)
{
    if ( enable )
    {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glEnable(GL_STENCIL_TEST);
    }
    else
    {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_STENCIL_TEST);
    }
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

int Application::GetMaxSlotForTextures()
{
    return MaxSlotForTextures;
}
