#include "TerrainModel.h"
#include "Config.h"
#include "TerrainGenerator.h"
#include "OpenGL/Mesh/Mesh.h"
#include "OpenGL/VertexBuffer/VertexBuffer.h"
#include "Core/Application.h"
#include "Library/Math.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include "OpenGL/Textures/Texture.h"

TerrainModel::TerrainModel(int width, float subdivisions, ShaderType shaderType)
    : Model({}, shaderType), m_Width(width), m_Subdivisions(subdivisions)
{
    Texture* defaultTexture = Application::Get()->GetBatchRenderer()->CreateOrGetTexture("res/textures/Dirt.jpg", Diffuse, m_ShaderType);
    
    CreateVertices();
    CalculIndices();

    Mesh* terrainMesh = new Mesh(vertices, indices,{*defaultTexture}, Mat4(1.0f));
    m_Meshes.emplace_back(std::unique_ptr<Mesh>(terrainMesh));
}

TerrainModel::~TerrainModel() = default;

void TerrainModel::CreateVertices() 
{
    vertices.clear();

    const float stepSize = m_Width / m_Subdivisions;
    
    for (int i = 0; i < m_Subdivisions; ++i)
    {
        for (int j = 0; j < m_Subdivisions; ++j)
        {
            Vec3<float> position;
            position.x = j * stepSize - m_Width / 2.0f;
            position.z = i * stepSize - m_Width / 2.0f;
            position.y = 0.0f; // Initial height is 0
            
            vertices.emplace_back(Vertex({ position }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { (float)j / m_Subdivisions, (float)i / m_Subdivisions }));
       }
    }
}

void TerrainModel::CalculIndices()
{
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
}

void TerrainModel::ReCalculateNormals(const std::vector<std::unique_ptr<Mesh>>::value_type& m_Mesh)
{
    // Clear existing normals
    for (auto& vertex : m_Mesh->m_Vertices)
    {
        vertex.m_Normal = {0.0f, 0.0f, 0.0f};
    }
    
    for (int i = 0; i < m_Mesh->m_Indices.size(); i += 3)
    {
        const int index1 = m_Mesh->m_Indices[i];
        const int index2 = m_Mesh->m_Indices[i + 1];
        const int index3 = m_Mesh->m_Indices[i + 2];

        Vec3<float> pos1 = m_Mesh->m_Vertices[index1].m_Position;
        Vec3<float> pos2 = m_Mesh->m_Vertices[index2].m_Position;
        Vec3<float> pos3 = m_Mesh->m_Vertices[index3].m_Position;
        const Vec3<float> faceNormal = Math::cross(pos2 - pos1, pos3 - pos1);

        m_Mesh->m_Vertices[index1].m_Normal += faceNormal;
        m_Mesh->m_Vertices[index2].m_Normal += faceNormal;
        m_Mesh->m_Vertices[index3].m_Normal += faceNormal;
    }

    // Normalize vertex normals
    for (auto& vertex : m_Mesh->m_Vertices)
    {
        vertex.m_Normal = Math::Normalize(vertex.m_Normal);
    }
}

void TerrainModel::SetHeight(const std::vector<float>& heightMap)
{
    const TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();

    int scale = 20;
    int t = 0;
    
    m_Meshes[0]->m_Indices.resize((m_Subdivisions - 1) * (m_Subdivisions - 1) * 6);
    m_Meshes[0]->m_Vertices.resize(m_Subdivisions * m_Subdivisions);
    
    for (int i = 0; i < m_Subdivisions * m_Subdivisions; ++i)
    {
        int x = i % (int)m_Subdivisions;
        int y = i / (int)m_Subdivisions;
        int m_MapSizeWithBorder = Application::Get()->GetTerrainGenerator()->GetSubdivisions() + 2 * 2;
        int brushRadius = 2;
        int borderedMapIndex = (y + brushRadius) * m_MapSizeWithBorder + x + brushRadius;
        int meshMapIndex = y * (int)m_Subdivisions + x;

        Vec2<float> percent = { (float)x / (m_Subdivisions-1.f), (float)y / (m_Subdivisions -1.f) };
        Vec3<float> pos = { percent.x * 2 - 1, 0, percent.y * 2 - 1 };
        pos *= scale;

        float normalizedHeight = heightMap[borderedMapIndex];
        pos += Vec3<float>::Up() * normalizedHeight * terrainGenerator->m_ElevationScale;
        m_Meshes[0]->m_Vertices[meshMapIndex].m_Position = pos;

        if (x != m_Subdivisions - 1 && y != m_Subdivisions - 1)
        {
            t = (y * (m_Subdivisions - 1) + x) * 3 * 2;

            m_Meshes[0]->m_Indices[t + 0] = meshMapIndex + m_Subdivisions;
            m_Meshes[0]->m_Indices[t + 1] = meshMapIndex + m_Subdivisions + 1;
            m_Meshes[0]->m_Indices[t + 2] = meshMapIndex;

            m_Meshes[0]->m_Indices[t + 3] = meshMapIndex + m_Subdivisions + 1;
            m_Meshes[0]->m_Indices[t + 4] = meshMapIndex + 1;
            m_Meshes[0]->m_Indices[t + 5] = meshMapIndex;
            t += 6;
        }
    }

    ReCalculateNormals(m_Meshes[0]);
}

void TerrainModel::ValidateTerrain()
{
    SendDataRender();
}