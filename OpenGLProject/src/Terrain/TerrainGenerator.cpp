#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator()
{
    modules.emplace_back(std::make_unique<PerlinNoiseModule>(0.05));
}
std::unique_ptr<Model> TerrainGenerator::GenerateTerrain(float width, float depth, int subdivisions,
                                                         ShaderType shaderType)
{
    TerrainModel* terrain = new TerrainModel(width-1, depth-1, subdivisions, shaderType);
    std::vector heightMap(subdivisions, std::vector<float>(subdivisions));

    //Apply Modules Modification
    for(const auto& module : modules){
        module->Process(heightMap);
    }

    //Set heighmap to the plane
    for (int x = 0; x < subdivisions; ++x)
    {
        for (int y = 0; y < subdivisions; ++y)
        {
            float value =  heightMap[x][y];
            terrain->SetVertexHeight(x, y,value);
        }
    }
    terrain->ValidateTerrain();
        
    return std::unique_ptr<Model>(terrain);
}
