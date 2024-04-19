#pragma once
#include "TerrainGenerationModule.h"
#include "TerrainModel.h"
#include "PerlinNoise/PerlinNoiseModule.h"

class TerrainGenerator
{
public:
    TerrainGenerator()
    {
        modules.push_back(&perlin);
    }
    std::unique_ptr<Model> GenerateTerrain(float width, float depth, int subdivisions, ShaderType shaderType)
    {
        TerrainModel* terrain = new TerrainModel(width-1, depth-1, subdivisions, shaderType);

        std::vector<std::vector<float>> heightMap(subdivisions, std::vector<float>(subdivisions));
        for(auto module : modules){
            module->Process(heightMap);
        }
        for (int x = 0; x < subdivisions; ++x)
        {
            for (int y = 0; y < subdivisions; ++y)
            {
                float value =  heightMap[x][y];
                terrain->SetVertexHeight(x, y,value*100.f);
            }
        }
        terrain->ValidateTerrain();
    
        return std::unique_ptr<Model>(terrain);
    }
private:
    PerlinNoiseModule perlin;
    std::vector<PerlinNoiseModule*> modules;
};
