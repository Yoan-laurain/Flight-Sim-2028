#pragma once
#include "Terrain/TerrainGenerationModule.h"

class PerlinNoiseModule : TerrainGenerationModule
{
public:
     void Process(std::vector<std::vector<float>>& heighmap) override;
     float _frequency = 0.05f;
};
