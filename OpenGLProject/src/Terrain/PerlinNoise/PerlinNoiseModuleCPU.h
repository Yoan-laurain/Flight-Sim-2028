#pragma once

#include "Terrain/TerrainGenerationModule.h"

class PerlinNoiseModuleCPU : public TerrainGenerationModule
{
    public:
        PerlinNoiseModuleCPU();
        ~PerlinNoiseModuleCPU() override = default;
    
        void Process(std::vector<float>& heighmap) override;

        float m_Frequency = 0.05f;
};