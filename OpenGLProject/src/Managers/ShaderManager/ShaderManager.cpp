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
    CreateShader<Shader>(ShaderType::EROSION, "../../OpenGLProject/src/Shaders/Erosion.shader");
}

void ShaderManager::RunComputeShader(const Shader* shader, int numIterations, float wGroupSize) const
{
    Vec3 workGroupSize = { 1, 1, 1 };
    
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize.x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize.y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize.z);

    const int workGroupCountX = static_cast<int>(ceil(numIterations / wGroupSize));
    const int workGroupCountY = static_cast<int>(ceil(1 / (float)workGroupSize.y));
    const int workGroupCountZ = static_cast<int>(ceil(1 / (float)workGroupSize.y));
    
    glUseProgram(shader->m_ID);
    glDispatchCompute(workGroupCountX, workGroupCountY, workGroupCountZ);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
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