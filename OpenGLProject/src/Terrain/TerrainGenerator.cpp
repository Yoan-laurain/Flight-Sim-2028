#include "TerrainGenerator.h"
#include "TerrainModel.h"
#include "OpenGL/Shader/Shader.h"
#include "PerlinNoise/CPU/PerlinNoiseModuleCPU.h"
#include "PerlinNoise/GPU/PerlinNoiseModuleGPU.h"
#include "Terrain/TerrainGenerationModule.h"
#include "Erosion/ErosionModuleGPU.h"

TerrainGenerator::TerrainGenerator() : m_subdivision(0)
{
    UpdateGenerationModules();
}

TerrainGenerator::~TerrainGenerator() = default;

void TerrainGenerator::GenerateTerrain(float width, int subdivisions, ShaderType shaderType)
{
    m_subdivision = subdivisions;
    m_terrain = std::make_unique<TerrainModel>(width, subdivisions, shaderType);
    UpdateTerrain();
}

void TerrainGenerator::UpdateTerrain() const
{
    std::vector<float> heightMap(m_subdivision*m_subdivision);
    
    for(const auto& module : m_modules)
    {
        module->Process(heightMap);
    }
    
    m_terrain->SetHeight(heightMap);
}

void TerrainGenerator::UpdateGenerationModules()
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

TerrainModel* TerrainGenerator::GetTerrain() const
{
    return m_terrain.get();
}