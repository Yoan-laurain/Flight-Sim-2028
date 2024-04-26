#pragma once

#include <vector>
#include <memory>

class TerrainGenerationModule;
class TerrainModel;
enum class ShaderType;
class Shader;

class TerrainGenerator
{
    public:
        TerrainGenerator();
        ~TerrainGenerator();
    
        void GenerateTerrain(float width, int subdivisions, ShaderType shaderType);
        void CalculateTerrain();
        void Run(const Shader* shader, int numIterations = 0, float wGroupSize = 64);
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

        float m_ElevationScale = 10.f;
        bool m_GenerateGPU = true;

        int GetSubdivisions() const;
    
    private:
        int m_subdivision;
        std::unique_ptr<TerrainModel> m_terrain;
        std::vector<std::unique_ptr<TerrainGenerationModule>> m_modules;
};