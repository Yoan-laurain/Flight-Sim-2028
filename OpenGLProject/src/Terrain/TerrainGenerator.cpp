#include "TerrainGenerator.h"

#include "TerrainModel.h"
#include "PerlinNoise/PerlinNoiseModuleCPU.h"
#include "PerlinNoise/PerlinNoiseModuleGPU.h"
#include "Terrain/TerrainGenerationModule.h"

TerrainGenerator::TerrainGenerator() : m_subdivision(0)
{
    UpdateGenerationMode();
}

TerrainGenerator::~TerrainGenerator() = default;

void TerrainGenerator::GenerateTerrain(float width, float depth, int subdivisions,ShaderType shaderType)
{
    m_subdivision = subdivisions;
    m_terrain = std::make_unique<TerrainModel>(width, depth, subdivisions-1, shaderType);
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
}
