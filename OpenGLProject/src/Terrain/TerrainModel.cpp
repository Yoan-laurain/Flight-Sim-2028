﻿#include "TerrainModel.h"
#include "Config.h"
#include "TerrainGenerator.h"
#include "OpenGL/VertexBuffer/VertexBuffer.h"
#include "Core/Application.h"
#include "Library/Math.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include "Mesh/Mesh.h"
#include "OpenGL/Textures/Texture.h"
#include "PerlinNoise/PerlinNoiseBaseModule.h"

TerrainModel::TerrainModel(const int width, const float subdivisions, const ShaderType shaderType)
    : Model({}, shaderType), m_Width(width), m_Subdivisions(subdivisions)
{
}

TerrainModel::~TerrainModel() = default;

void TerrainModel::SetHeight(const std::vector<float>& heightMap)
{
    m_vertices.resize(heightMap.size());
    m_indices.resize(heightMap.size() * 6);

    const int size = static_cast<int>(m_Subdivisions) * static_cast<int>(m_Subdivisions);
    
    for (int i = 0; i < size; ++i)
    {
        // Retrieve the x and y coordinates of the vertex
        const int x = i % static_cast<int>(m_Subdivisions);
        const int y = i / static_cast<int>(m_Subdivisions);

        const int borderedMapIndex = (y + ErosionBrushRadius) * Application::Get()->GetTerrainGenerator()->m_BorderedMapSize + x + ErosionBrushRadius;
        const int meshMapIndex = y * static_cast<int>(m_Subdivisions) + x;
        
        UpdateVertexProperties(meshMapIndex, x, y, heightMap, borderedMapIndex);
        GenerateIndices(x, y, meshMapIndex);
    }

    ReCalculateNormals();
    UpdateOrCreateNewMesh();
}

void TerrainModel::ReCalculateNormals()
{
    // Clear existing normals
    for (auto& vertex : m_vertices)
    {
        vertex.m_Normal = {0.0f, 0.0f, 0.0f};
    }

    const int size = static_cast<int>(m_indices.size());
    for (int i = 0; i < size; i += 3)
    {
        const unsigned int index1 = m_indices[i];
        const unsigned int index2 = m_indices[i + 1];
        const unsigned int index3 = m_indices[i + 2];

        Vec3<float> pos1 = m_vertices[index1].m_Position;
        Vec3<float> pos2 = m_vertices[index2].m_Position;
        Vec3<float> pos3 = m_vertices[index3].m_Position;
        const Vec3<float> faceNormal = Math::cross(pos2 - pos1, pos3 - pos1);

        m_vertices[index1].m_Normal += faceNormal;
        m_vertices[index2].m_Normal += faceNormal;
        m_vertices[index3].m_Normal += faceNormal;
    }

    // Normalize vertex normals
    for (auto& vertex : m_vertices)
    {
        vertex.m_Normal = Math::Normalize(vertex.m_Normal);
    }
}

void TerrainModel::GenerateIndices(const int x, const int y, const int meshMapIndex)
{
    if (x != m_Subdivisions - 1 && y != m_Subdivisions - 1)
    {
        const int baseIndex = (y * (m_Subdivisions - 1) + x) * 6;
        const int vertexIndex = meshMapIndex;

        m_indices[baseIndex + 0] = vertexIndex + m_Subdivisions;
        m_indices[baseIndex + 1] = vertexIndex + m_Subdivisions + 1;
        m_indices[baseIndex + 2] = vertexIndex;

        m_indices[baseIndex + 3] = vertexIndex + m_Subdivisions + 1;
        m_indices[baseIndex + 4] = vertexIndex + 1;
        m_indices[baseIndex + 5] = vertexIndex;
    }
}

void TerrainModel::UpdateOrCreateNewMesh()
{
    if (m_Meshes.empty())
    {
        Texture* defaultTexture = Application::Get()->GetBatchRenderer()->CreateOrGetTexture(GRASS_TEXTURE_PATH, Diffuse, m_ShaderType);
        Texture* rockTexture = Application::Get()->GetBatchRenderer()->CreateOrGetTexture(STONE_TEXTURE_PATH, Diffuse, m_ShaderType);
        Texture* snowTexture = Application::Get()->GetBatchRenderer()->CreateOrGetTexture(SNOW_TEXTURE_PATH, Diffuse, m_ShaderType);
        Texture* dirtTexture = Application::Get()->GetBatchRenderer()->CreateOrGetTexture(DIRT_TEXTURE_PATH, Diffuse, m_ShaderType);
        
        Mesh* terrainMesh = new Mesh(m_vertices, m_indices, {*defaultTexture,*snowTexture,*dirtTexture,*rockTexture}, Mat4(1.0f));
        m_Meshes.emplace_back(std::unique_ptr<Mesh>(terrainMesh));
    }
    else
    {
        m_Meshes[0]->m_Vertices = m_vertices;
        m_Meshes[0]->m_Indices = m_indices;
    }
}

void TerrainModel::UpdateVertexProperties(const int meshMapIndex, const int x, const int y, const std::vector<float>& heightMap, const int borderedMapIndex)
{
    // Calculate vertex position
    const Vec2 percent = { static_cast<float>(x) / (m_Subdivisions - 1.f), static_cast<float>(y) / (m_Subdivisions - 1.f) }; 
    Vec3<float> pos = { percent.x * 2 - 1, 0, percent.y * 2 - 1 };
    pos *= Application::Get()->GetTerrainGenerator()->GetModule<PerlinNoiseBaseModule>()->GetScale();
    pos += Vec3<float>::Up() * heightMap[borderedMapIndex] * Application::Get()->GetTerrainGenerator()->m_ElevationScale;
    
    m_vertices[meshMapIndex].m_Position = pos;
    m_vertices[meshMapIndex].m_Color = {1.0f, 1.0f, 1.0f};
    m_vertices[meshMapIndex].m_TexUV = {percent.x*10, percent.y*10};
    
    if (!m_Meshes.empty())
    {
        m_vertices[meshMapIndex].m_IndexModel = m_Meshes[0]->m_Vertices.begin()->m_IndexModel;
        m_vertices[meshMapIndex].m_IndexDiffuse = m_Meshes[0]->m_Vertices.begin()->m_IndexDiffuse;
        m_vertices[meshMapIndex].m_IndexSpecular = m_Meshes[0]->m_Vertices.begin()->m_IndexSpecular;
    }
}