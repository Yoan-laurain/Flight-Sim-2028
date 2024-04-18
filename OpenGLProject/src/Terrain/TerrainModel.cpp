#include "TerrainModel.h"

#include "Config.h"
#include "../OpenGL/Mesh/Mesh.h"
#include "../OpenGL/VertexBuffer/VertexBuffer.h"
#include "../Core/Application.h"
#include "../Library/Math.h"
#include "Managers/BatchRenderer/BatchRenderer.h"

TerrainModel::TerrainModel(float width, float depth, int subdivisions, ShaderType shaderType)
    : Model({}, shaderType), m_Width(width), m_Depth(depth), m_Subdivisions(subdivisions)
{
    float stepSizeX = m_Width / m_Subdivisions;
    float stepSizeZ = m_Depth / m_Subdivisions;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
   
    Mat4<float> matrix(1.0f);
    
    for (int i = 0; i <= m_Subdivisions; ++i)
    {
        for (int j = 0; j <= m_Subdivisions; ++j)
        {
            float x = j * stepSizeX - m_Width / 2.0f;
            float z = i * stepSizeZ - m_Depth / 2.0f;
            float y = 0.0f; // Initial height is 0

            vertices.push_back(Vertex({ x, y, z }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { (float)j / m_Subdivisions, (float)i / m_Subdivisions }));
        }
    }
    for (int i = 0; i < m_Subdivisions; ++i)
    {
        for (int j = 0; j < m_Subdivisions; ++j)
        {
            const int topLeft = i * (m_Subdivisions + 1) + j;
            const int topRight = topLeft + 1;
            const int bottomLeft = (i + 1) * (m_Subdivisions + 1) + j;
            const int bottomRight = bottomLeft + 1;
            indices.push_back(topLeft);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(bottomRight);
            indices.push_back(bottomLeft);
        }
    }

    std::vector<Texture> textures;
    const Texture* defaultTexture = Application::Get()->GetBatchRenderer()->CreateOrGetTexture("res/textures/grass.png", Diffuse, m_ShaderType);
    textures.push_back(*defaultTexture);
    Mesh* terrainMesh = new Mesh(vertices, indices,textures, matrix);
    m_Meshes.push_back(std::unique_ptr<Mesh>(terrainMesh));
}

TerrainModel::~TerrainModel() = default;

void TerrainModel::ValidateTerrain()
{
    SendDataRender();
}

void TerrainModel::SetVertexHeight(int x, int z, float height) const
{
    if (x >= 0 && x <= m_Subdivisions && z >= 0 && z <= m_Subdivisions)
    {
        const int index = z * (m_Subdivisions + 1) + x;
        m_Meshes[0]->m_Vertices[index].m_Position.y = height;
    }
}
