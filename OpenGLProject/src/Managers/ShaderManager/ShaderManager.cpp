#include "ShaderManager.h"
#include "OpenGL/Shader/Shader.h"
#include "OpenGL/Shader/Basic/BasicShader.h"
#include "OpenGL/Shader/SkyBox/SkyBoxShader.h"
#include "OpenGL/Shader/Terrain/TerrainShader.h"
#include <stdexcept>
#include <GL/glew.h>

ShaderManager::ShaderManager()
{
    CreateShader<BasicShader>(ShaderType::BASIC, BASIC_SHADER_PATH);
    CreateShader<TerrainShader>(ShaderType::TERRAIN, TERRAIN_SHADER_PATH);
    CreateShader<SkyBoxShader>(ShaderType::SKYBOX, SKYBOX_SHADER_PATH);
    CreateShader<Shader>(ShaderType::HEIGHTMAP, HEIGHTMAP_SHADER_PATH);
    CreateShader<Shader>(ShaderType::EROSION, EROSION_SHADER_PATH);
}

void ShaderManager::RunComputeShader(const Shader* shader, const int numIterations, const float wGroupSize) const
{
    Vec3 workGroupSize = { 1, 1, 1 };
    
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize.x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize.y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize.z);

    const int workGroupCountX = static_cast<int>(ceil(static_cast<float>(numIterations) / wGroupSize));
    const int workGroupCountY = static_cast<int>(ceil(1 / static_cast<float>(workGroupSize.y)));
    const int workGroupCountZ = static_cast<int>(ceil(1 / static_cast<float>(workGroupSize.y)));
    
    glUseProgram(shader->m_ID);
    glDispatchCompute(workGroupCountX, workGroupCountY, workGroupCountZ);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ShaderManager::RegisterShader(const ShaderType shaderType, Shader* shader)
{
    if (!m_shaders.contains(shaderType))
    {
        m_shaders[shaderType] = std::unique_ptr<Shader>(shader);
        return;
    }
    throw std::runtime_error("Shader already exists in the ShaderManager");
}

Shader* ShaderManager::GetShader(const ShaderType shaderType)
{
    if (m_shaders.contains(shaderType))
    {
        return m_shaders[shaderType].get();
    }
    throw std::runtime_error("Shader does not exist in the ShaderManager");
}