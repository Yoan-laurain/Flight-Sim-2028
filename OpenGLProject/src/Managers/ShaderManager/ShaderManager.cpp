#include "ShaderManager.h"
#include "../../OpenGL/Shader/Shader.h"
#include "../../OpenGL/Shader/Basic/BasicShader.h"
#include "../../OpenGL/Shader/SkyBox/SkyBoxShader.h"
#include <stdexcept>

ShaderManager::ShaderManager()
{
    CreateShader<BasicShader>(ShaderType::BASIC, "../../OpenGLProject/src/Shaders/Basic.shader");
    CreateShader<SkyBoxShader>(ShaderType::SKYBOX, "../../OpenGLProject/src/Shaders/Skybox.shader");
    CreateShader<Shader>(ShaderType::HEIGHTMAP, "../../OpenGLProject/src/Shaders/HeightMap.shader");
}

void ShaderManager::RegisterShader(const ShaderType shaderType, Shader* shader)
{
    if (!m_Shaders.contains(shaderType))
    {
        m_Shaders[shaderType] = std::unique_ptr<Shader>(shader);
    }
    else
    {
        throw std::runtime_error("Shader already exists in the ShaderManager");
    }
}

Shader* ShaderManager::GetShader(const ShaderType shaderType)
{
    if (m_Shaders.contains(shaderType))
    {
        return m_Shaders[shaderType].get();
    }
    
    throw std::runtime_error("Shader does not exist in the ShaderManager");
}