#include "TerrainGenerator.h"
#include "TerrainModel.h"
#include "OpenGL/Shader/Shader.h"
#include "PerlinNoise/CPU/PerlinNoiseModuleCPU.h"
#include "PerlinNoise/GPU/PerlinNoiseModuleGPU.h"
#include "Terrain/TerrainGenerationModule.h"
#include <GL/glew.h>

#include "Erosion/ErosionModuleGPU.h"

TerrainGenerator::TerrainGenerator() : m_subdivision(0)
{
    UpdateGenerationMode();
}

TerrainGenerator::~TerrainGenerator() = default;

void TerrainGenerator::GenerateTerrain(float width, int subdivisions,ShaderType shaderType)
{
    m_subdivision = subdivisions;
    m_terrain = std::make_unique<TerrainModel>(width, subdivisions, shaderType);
    CalculateTerrain();
}

void TerrainGenerator::CalculateTerrain()
{
    std::vector<float> heightMap(m_subdivision*m_subdivision);
    
    for(const auto& module : m_modules){
        module->Process(heightMap);
    }
    
    m_terrain->SetHeight(heightMap);
}

void TerrainGenerator::Run(const Shader* shader, int numIterations, float wGroupSize)
{
    Vec3 workGroupSize = { 1, 1, 1 };
    
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize.x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize.y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize.z);
    
    int workGroupCountX = static_cast<int>(ceil(numIterations / wGroupSize));
    int workGroupCountY = static_cast<int>(ceil(1 / (float)workGroupSize.y));
    int workGroupCountZ = static_cast<int>(ceil(1 / (float)workGroupSize.y));
    
    glUseProgram(shader->m_ID);
    glDispatchCompute(workGroupCountX, workGroupCountY, workGroupCountZ);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

TerrainModel* TerrainGenerator::GetTerrain()
{
    return m_terrain.get();
}

void TerrainGenerator::UpdateGenerationMode()
{
    m_modules.clear();
    if(m_GenerateGPU)
    {
        m_modules.emplace_back(std::make_unique<PerlinNoiseModuleGPU>());   
    }
    else
    {
        m_modules.emplace_back(std::make_unique<PerlinNoiseModuleCPU>());
    }

    m_modules.emplace_back( std::make_unique<ErosionModuleGPU>());
}

int TerrainGenerator::GetSubdivisions() const
{
    return m_subdivision;
}
