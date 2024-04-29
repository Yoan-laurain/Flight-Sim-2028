#pragma once

#include "Terrain/PerlinNoise/PerlinNoiseBaseModule.h"

class PerlinNoiseModuleCPU : public PerlinNoiseBaseModule
{
    public:
        PerlinNoiseModuleCPU();
        ~PerlinNoiseModuleCPU() override = default;

        /* Inherited via TerrainGenerationModule */
        void Process(std::vector<float>& heighmap) override;
        /* Inherited via TerrainGenerationModule */

        float m_Frequency = 0.05f;
};
