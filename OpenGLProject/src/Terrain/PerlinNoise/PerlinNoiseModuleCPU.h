#pragma once
#include <random>

#include "Terrain/TerrainGenerationModule.h"


class PerlinNoiseModuleCPU : public TerrainGenerationModule
{
public:
    explicit PerlinNoiseModuleCPU();
     ~PerlinNoiseModuleCPU() override = default;
     void Process(std::vector<float>& heighmap) override;

    float m_Frequency = 0.05f;
    
};

