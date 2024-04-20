#pragma once
#include "Terrain/TerrainGenerationModule.h"

class PerlinNoiseModule : public TerrainGenerationModule
{
public:
    explicit PerlinNoiseModule(float frequency);
      PerlinNoiseModule() = delete;    
     ~PerlinNoiseModule() override = default;
     void Process(std::vector<std::vector<float>>& heighmap) override;
private:
     float _frequency = 0.05f;
};
