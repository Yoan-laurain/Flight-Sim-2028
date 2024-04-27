#pragma once

#include "Terrain/TerrainGenerationModule.h"

class PerlinNoiseModuleCPU : public TerrainGenerationModule
{
    public:
        PerlinNoiseModuleCPU();
        ~PerlinNoiseModuleCPU() override = default;

        /* Inherited via TerrainGenerationModule */
        void Process(std::vector<float>& heighmap) override;
        /* Inherited via TerrainGenerationModule */

        float m_Frequency = 0.05f;
};