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

        /* Generate the terrain with the given width and subdivisions */
        void GenerateTerrain(float width, int subdivisions, ShaderType shaderType);

        /*  Update the terrain by processing all the modules generating the new heightmap and updating the terrain model */
        void UpdateTerrain() const;

        /*  Update the generation modules based on the current settings */
        void UpdateGenerationModules();
    
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
    
        TerrainModel* GetTerrain() const;
        int GetSubdivisions() const;

        float m_ElevationScale = 20.f;
        bool m_GenerateGPU = true;
        int m_BorderedMapSize;

        double GetMinGenerationTime() const;
        double GetAverageGenerationTime() const;
        double GetMaxGenerationTime() const;
    
    private:
        int m_subdivision;
        std::unique_ptr<TerrainModel> m_terrain;
        std::vector<std::unique_ptr<TerrainGenerationModule>> m_modules;
};