#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../OpenGL/Mesh/Mesh.h"

enum class ShaderType;

class Terrain
{
public:
    Terrain(float size, int subdivisions,ShaderType shaderType);
    ~Terrain();

    void Update();

    void SetVertexHeight(int x, int z, float height);
    void SetTexture(const std::vector<Texture>& textures);
    int GetNumberOfTriangles();

    glm::vec3 m_Rotation;
    glm::vec3 m_Translation;
    glm::vec3 m_Scale;
    ShaderType m_ShaderType;
private:
    std::vector<Vertex> GenerateVertices(float size, int subdivisions);
    std::vector<unsigned int> GenerateIndices(int subdivisions);
    
    std::unique_ptr<Mesh> m_Mesh;
    float m_Size;
    int m_Subdivisions;
};
