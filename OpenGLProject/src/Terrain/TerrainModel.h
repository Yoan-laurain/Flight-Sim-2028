#pragma once

#include "Model/Model.h"

struct Vertex;
enum class ShaderType;

class TerrainModel : public Model
{
    public:
        TerrainModel(int width, float subdivisions, ShaderType shaderType);
        virtual ~TerrainModel();

        void ValidateTerrain();
        void SetHeight(const std::vector<float>& heightMap);
        
        int m_Width;
        float m_Subdivisions;

    private:
        void CreateVertices();
        void CalculIndices();
        void ReCalculateNormals(const std::vector<std::unique_ptr<Mesh>>::value_type& m_Mesh);

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
};