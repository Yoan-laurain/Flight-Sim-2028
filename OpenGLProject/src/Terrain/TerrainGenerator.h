#pragma once

#include <vector>
#include <memory>

#include "Model/Model.h"

class TerrainGenerationModule;
class ShaderStorageBufferObject;
class TerrainModel;

class TerrainGenerator
{
    public:
    TerrainGenerator();
    ~TerrainGenerator();
    void GenerateTerrain(float width, float depth, int subdivisions, ShaderType shaderType);
    void CalculateTerrain();
    TerrainModel* GetTerrain();
    
    template<typename ModuleType>
    ModuleType* GetModule()
    {
        for (const auto& module : m_modules)
        {
            ModuleType* castedModule = dynamic_cast<ModuleType*>(module.get());
            if (castedModule)
            {
                return castedModule;
            }
        }
        return nullptr;
    }
    void UpdateGenerationMode();

    float m_ElevationScale = 100.f;
    bool m_GenerateGPU = false;
    
    private:
        int m_subdivision;
        std::unique_ptr<TerrainModel> m_terrain;
        std::vector<std::unique_ptr<TerrainGenerationModule>> m_modules;
};