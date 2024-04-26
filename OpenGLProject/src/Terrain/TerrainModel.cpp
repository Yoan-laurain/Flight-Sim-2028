#include "TerrainModel.h"
#include "Config.h"
#include "TerrainGenerator.h"
#include "OpenGL/Mesh/Mesh.h"
#include "OpenGL/VertexBuffer/VertexBuffer.h"
#include "Core/Application.h"
#include "Library/Math.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include "OpenGL/Textures/Texture.h"

TerrainModel::TerrainModel(float width, float depth, int subdivisions, ShaderType shaderType)
    : Model({}, shaderType), m_Width(width), m_Depth(depth), m_Subdivisions(subdivisions)
{
    Texture* defaultTexture = Application::Get()->GetBatchRenderer()->CreateOrGetTexture("res/textures/grass.png", Diffuse, m_ShaderType);
    Texture* rockTexture = Application::Get()->GetBatchRenderer()->CreateOrGetTexture("res/textures/stone.jpg", Diffuse, m_ShaderType);
    CreateVertices();
    CalculIndices();

    Mesh* terrainMesh = new Mesh(vertices, indices,{*defaultTexture,*rockTexture}, Mat4(1.0f));
    m_Meshes.emplace_back(std::unique_ptr<Mesh>(terrainMesh));
}

TerrainModel::~TerrainModel() = default;

void TerrainModel::CreateVertices() 
{
    vertices.clear();
    
    float stepSizeX = m_Width / m_Subdivisions;
    float stepSizeZ = m_Depth / m_Subdivisions;
    
    for (int i = 0; i <= m_Subdivisions; ++i)
    {
        for (int j = 0; j <= m_Subdivisions; ++j)
        {
            Vec3<float> position;
            position.x = j * stepSizeX - m_Width / 2.0f;
            position.z = i * stepSizeZ - m_Depth / 2.0f;
            position.y = 0.0f; // Initial height is 0

            vertices.emplace_back(Vertex({ position }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { (float)j / m_Subdivisions, (float)i / m_Subdivisions }));
            baseVertices.emplace_back(Vertex({ position }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { (float)j / m_Subdivisions, (float)i / m_Subdivisions }));
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
    for (auto& vertex : m_Mesh->m_Vertices)
    {
        vertex.m_Normal = Vec3<float>(0.0f, 0.0f, 0.0f);
    }
    
    for (int i = 0; i < m_Mesh->m_Indices.size(); i += 3)
    {
        int index1 = m_Mesh->m_Indices[i];
        int index2 = m_Mesh->m_Indices[i + 1];
        int index3 = m_Mesh->m_Indices[i + 2];


        Vec3<float> pos1 = m_Mesh->m_Vertices[index1].m_Position;
        Vec3<float> pos2 = m_Mesh->m_Vertices[index2].m_Position;
        Vec3<float> pos3 = m_Mesh->m_Vertices[index3].m_Position;
        Vec3<float> faceNormal = Math::cross(pos2 - pos1, pos3 - pos1);
        
        m_Mesh->m_Vertices[index1].m_Normal += faceNormal;
        m_Mesh->m_Vertices[index2].m_Normal += faceNormal;
        m_Mesh->m_Vertices[index3].m_Normal += faceNormal;
    }
    
    for (auto& vertex : m_Mesh->m_Vertices)
    {
        vertex.m_Normal = Math::Normalize(vertex.m_Normal);
    }
}


void TerrainModel::SetHeight(const std::vector<float>& heightMap)
{
    TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
    
    for (const auto& m_Mesh : m_Meshes)
    {
        ReCalculateNormals(m_Mesh);
        
        for (int i = 0; i < m_Mesh->m_Vertices.size(); ++i)
        {
            m_Mesh->m_Vertices[i].m_Position = baseVertices[i].m_Position;

            const float normalizedHeight = heightMap[i];
            
            m_Mesh->m_Vertices[i].m_Position.y += normalizedHeight * terrainGenerator->m_ElevationScale;
        }
    }
}

void TerrainModel::ValidateTerrain()
{
    SendDataRender();
}