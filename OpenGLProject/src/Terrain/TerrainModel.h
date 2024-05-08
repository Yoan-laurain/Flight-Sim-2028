#pragma once

#include "Model/Model.h"

struct Vertex;
enum class ShaderType;

#define GRASS_TEXTURE_PATH "res/textures/grass.png"
#define STONE_TEXTURE_PATH "res/textures/stone.jpg"
#define SNOW_TEXTURE_PATH "res/textures/snow.png"
#define DIRT_TEXTURE_PATH "res/textures/dirt.png"

class TerrainModel : public Model
{
    public:
        TerrainModel(int width, float subdivisions, ShaderType shaderType);
        ~TerrainModel() override;
    
        void SetHeight(const std::vector<float>& heightMap);
        
        int m_Width;
        float m_Subdivisions;

    private:

        void ReCalculateNormals();
        void UpdateOrCreateNewMesh();
        void GenerateIndices(int x, int y, int meshMapIndex);
        void UpdateVertexProperties(int meshMapIndex, int x, int y, const std::vector<float>& heightMap, int borderedMapIndex);

        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
};