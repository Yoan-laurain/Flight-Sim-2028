#include "TerrainGenerator.h"
#include "Config.h"
#include "TerrainModel.h"
#include "PerlinNoise/CPU/PerlinNoiseModuleCPU.h"
#include "PerlinNoise/GPU/PerlinNoiseModuleGPU.h"
#include "Terrain/TerrainGenerationModule.h"
#include "Erosion/ErosionModuleGPU.h"
#include "Core/Application.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include <chrono>

TerrainGenerator::TerrainGenerator()
    : m_ElevationScale(20.f)
    , m_GenerateGPU(true)
    , m_BorderedMapSize(0)
    , m_subdivision(0)
    , m_isInitialized(false)
{
    m_modules.emplace_back(std::make_unique<PerlinNoiseModuleGPU>());
    m_modules.emplace_back(std::make_unique<ErosionModuleGPU>());
}

TerrainGenerator::~TerrainGenerator() = default;

void TerrainGenerator::GenerateTerrain(float width, int subdivisions, ShaderType shaderType)
{
    m_subdivision = subdivisions + 1; // TODO : comprendre pourquoi +1
    m_BorderedMapSize = m_subdivision + 2 * ErosionBrushRadius;
    m_terrain = std::make_unique<TerrainModel>(width, m_subdivision, shaderType);
    UpdateTerrain();
}

void TerrainGenerator::UpdateTerrain()
{
    const int count = m_subdivision * m_subdivision;
    std::vector<float> heightMap(count);

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
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        module->UpdateTimers(static_cast<double>(duration));
    }
    
    m_terrain->SetHeight(heightMap);

    if(!m_isInitialized)
    {
        m_terrain->SendDataRender();
        m_isInitialized = true;
        return;
    }
    
    Application::Get()->GetBatchRenderer()->UpdateModelData(m_terrain.get(),m_terrain->m_ShaderType, UpdateType::Vertices);
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

    UpdateTerrain();
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
        minTime = module->m_MinGenerationTime < minTime ? module->m_MinGenerationTime : minTime;
    }
    return minTime;
}

double TerrainGenerator::GetAverageGenerationTime() const
{
    double medTime = 0;
    for(const auto& module : m_modules)
    {
        medTime = module->GetAverageGenerationTime();
    }
    return medTime / static_cast<double>(m_modules.size());
}

double TerrainGenerator::GetMaxGenerationTime() const
{
    double maxTime = 0;
    for(const auto& module : m_modules)
    {
        maxTime = module->m_MaxGenerationTime > maxTime ? module->m_MaxGenerationTime : maxTime;
    }
    return maxTime;
}

TerrainModel* TerrainGenerator::GetTerrain() const
{
    return m_terrain.get();
}