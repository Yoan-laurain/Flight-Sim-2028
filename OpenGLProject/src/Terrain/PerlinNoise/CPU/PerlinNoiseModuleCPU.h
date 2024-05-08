#pragma once

#include "Terrain/PerlinNoise/PerlinNoiseBaseModule.h"

class PerlinNoiseModuleCPU : public PerlinNoiseBaseModule
{
    public:
        PerlinNoiseModuleCPU();
        ~PerlinNoiseModuleCPU() override = default;

        /* Inherited via TerrainGenerationModule */
        void Process(std::vector<float>& heightmap) override;
        /* Inherited via TerrainGenerationModule */
};
