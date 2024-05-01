#include "TerrainGenerator.h"
#include "Config.h"
#include "TerrainModel.h"
#include "OpenGL/Shader/Shader.h"
#include "PerlinNoise/CPU/PerlinNoiseModuleCPU.h"
#include "PerlinNoise/GPU/PerlinNoiseModuleGPU.h"
#include "Terrain/TerrainGenerationModule.h"
#include "Erosion/ErosionModuleGPU.h"
#include <chrono>

TerrainGenerator::TerrainGenerator() : m_subdivision(0)
{
    UpdateGenerationModules();
}

TerrainGenerator::~TerrainGenerator() = default;

void TerrainGenerator::GenerateTerrain(float width, int subdivisions, ShaderType shaderType)
{
    m_subdivision = subdivisions + 1;
    m_BorderedMapSize = m_subdivision + 2 * ErosionBrushRadius;
    m_terrain = std::make_unique<TerrainModel>(width, m_subdivision, shaderType);
    UpdateTerrain();
}

void TerrainGenerator::UpdateTerrain() const
{
    std::vector<float> heightMap(m_subdivision*m_subdivision);

    bool isPreviousModuleDirty = false;
    for(const auto& module : m_modules)
    {
        auto start = std::chrono::high_resolution_clock::now();
        
        if(isPreviousModuleDirty)
        {
            module->SetDirty();
        }
        isPreviousModuleDirty = module->IsDirty();
        module->Generate(heightMap);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        module->m_AverageGenerationTimeHistory.push_back(duration);

        module->m_MinGenerationTime = (duration < module->m_MinGenerationTime) ? duration : module->m_MinGenerationTime;
        module->m_AverageGenerationTime = module->GetAverageGenerationTime();
        module->m_MaxGenerationTime = (duration > module->m_MaxGenerationTime) ? duration : module->m_MaxGenerationTime;
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

double TerrainGenerator::GetMinGenerationTime() const
{
    double minTime = std::numeric_limits<double>::max();
    for(const auto& module : m_modules)
    {
        minTime = (module->m_MinGenerationTime < minTime) ? module->m_MinGenerationTime : minTime;
    }
    return minTime;
}

double TerrainGenerator::GetAverageGenerationTime() const
{
    double medTime = 0;
    for(const auto& module : m_modules)
    {
        module->m_AverageGenerationTime = module->GetAverageGenerationTime();
    }
    return medTime / m_modules.size();
}

double TerrainGenerator::GetMaxGenerationTime() const
{
    double maxTime = 0;
    for(const auto& module : m_modules)
    {
        maxTime = (module->m_MaxGenerationTime > maxTime) ? module->m_MaxGenerationTime : maxTime;
    }
    return maxTime;
}

TerrainModel* TerrainGenerator::GetTerrain() const
{
    return m_terrain.get();
}
