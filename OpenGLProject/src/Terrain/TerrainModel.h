#pragma once

#include "Model/Model.h"

struct Vertex;
enum class ShaderType;

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

        std::vector<Vertex> m_Vertices;
        std::vector<unsigned int> m_Indices;
};