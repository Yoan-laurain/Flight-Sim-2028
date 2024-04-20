#pragma once
#include "TerrainGenerationModule.h"
#include "TerrainModel.h"
#include "PerlinNoise/PerlinNoiseModule.h"

class TerrainGenerator
{
public:
    TerrainGenerator();
    std::unique_ptr<Model> GenerateTerrain(float width, float depth, int subdivisions, ShaderType shaderType)

private:
    std::vector<std::unique_ptr<TerrainGenerationModule>> modules;
};
