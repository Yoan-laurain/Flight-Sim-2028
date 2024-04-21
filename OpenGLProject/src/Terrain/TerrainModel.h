#pragma once
#include "Model/Model.h"

struct Vertex;
enum class ShaderType;

class TerrainModel : public Model
{
    public:
        TerrainModel(float width, float depth, int subdivisions, ShaderType shaderType);
        virtual ~TerrainModel();

        void ValidateTerrain();
        void SetHeight(const std::vector<float>& heightMap);
        
        float m_Width;
        float m_Depth;
        int m_Subdivisions;

    private:
        void CreateVertices();
        void CalculIndices();

        std::vector<Vertex> vertices;
        std::vector<Vertex> baseVertices;
        std::vector<unsigned int> indices;
};