#pragma once

#include <memory>
#include "Terrain/TerrainGenerationModule.h"

class ShaderStorageBufferObject;

class PerlinNoiseModuleGPU : public TerrainGenerationModule
{
    public:
        PerlinNoiseModuleGPU();
        ~PerlinNoiseModuleGPU() override;
        void Process(std::vector<float>& heighmap) override;
    
        // --------------- [ MODULABLES ] --------------- //
        int m_NumOctaves = 7;
        float m_Persistence = .5f;
        float m_Lacunarity = 4;
        float m_InitialScale = 20;

    private:
        void StartProcess(std::vector<float>& heighmap);
        void FinishProcess(std::vector<float>& heighmap);
    
        std::unique_ptr<ShaderStorageBufferObject> m_OffsetsBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_MapBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_MinMaxBuffer;
};
