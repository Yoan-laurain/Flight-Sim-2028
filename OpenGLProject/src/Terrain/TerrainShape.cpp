#include "TerrainShape.h"
#include <vector>
#include "../OpenGL/VertexBuffer/VertexBuffer.h"
#include "../OpenGL/Textures/Texture.h"
#include "../OpenGL/Mesh/Mesh.h"
#include <memory>

Terrain::Terrain(float size, int subdivisions, ShaderType shaderType) :
    m_Rotation(glm::vec3(0.0f))
    , m_Translation(glm::vec3(0.0f))
    , m_Scale(glm::vec3(1.0f)),
    m_Size(size), m_Subdivisions(subdivisions),m_ShaderType(shaderType)
{
    std::vector<Vertex> vertices = GenerateVertices(size, subdivisions);
    std::vector<unsigned int> indices = GenerateIndices(subdivisions);

    std::vector<Texture> textures = {};

    glm::mat4 model = glm::mat4(1.0f);

    m_Mesh = std::make_unique<Mesh>(vertices, indices, textures, model);
}

Terrain::~Terrain() = default;

void Terrain::SetVertexHeight(int x, int z, float height)
{
    // Assurez-vous que les coordonnées x et z sont dans les limites du terrain
    if (x >= 0 && x <= m_Subdivisions && z >= 0 && z <= m_Subdivisions)
    {
        // Calculez l'index du sommet dans le tableau en fonction des coordonnées x et z
        int index = z * (m_Subdivisions + 1) + x;

        // Accédez aux données des sommets dans le mesh et modifiez la hauteur du sommet à l'index spécifié
        //auto& vertices = m_Mesh->m_VBO->m_Vertices;
        //vertices[index].position.y = height;

        // Après avoir modifié les données des sommets, mettez à jour le Vertex Buffer avec les nouvelles données
        //m_Mesh->m_VBO->Bind();
        //glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(Vertex), sizeof(Vertex), &vertices[index]);
    }
}

void Terrain::SetTexture(const std::vector<Texture>& textures)
{
    
    m_Mesh->m_Textures = textures;
}

void Terrain::Update()
{
    m_Mesh->m_Transform = m_Translation;
    m_Mesh->m_Rotation = glm::quat(glm::radians(m_Rotation));
    m_Mesh->m_Scale = m_Scale;
}

int Terrain::GetNumberOfTriangles()
{
    // Le nombre total de quads est (subdivisions * subdivisions)
    int numQuads = m_Subdivisions * m_Subdivisions;

    // Chaque quad est formé de deux triangles
    return numQuads * 2;
}

std::vector<Vertex> Terrain::GenerateVertices(float size, int subdivisions)
{
    std::vector<Vertex> vertices;

    float step = size / subdivisions;
    float halfSize = size / 2.0f;

    for (int i = 0; i <= subdivisions; ++i)
    {
        for (int j = 0; j <= subdivisions; ++j)
        {
            float x = -halfSize + j * step;
            float z = -halfSize + i * step;
            float y = 0.0f;

            glm::vec3 position(x, y, z);
            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            glm::vec3 color(1.0f); // Default white color
            glm::vec2 texUV(static_cast<float>(j) / subdivisions, static_cast<float>(i) / subdivisions);

            vertices.emplace_back(position, normal, color, texUV);
        }
    }

    return vertices;
}

std::vector<unsigned int> Terrain::GenerateIndices(int subdivisions)
{
    std::vector<unsigned int> indices;

    for (int i = 0; i < subdivisions; ++i)
    {
        for (int j = 0; j < subdivisions; ++j)
        {
            int start = i * (subdivisions + 1) + j;
            indices.push_back(start);
            indices.push_back(start + 1);
            indices.push_back(start + subdivisions + 1);

            indices.push_back(start + 1);
            indices.push_back(start + subdivisions + 2);
            indices.push_back(start + subdivisions + 1);
        }
    }

    return indices;
}
