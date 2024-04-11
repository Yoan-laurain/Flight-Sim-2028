#pragma once

#include "../Library/coreMinimal.h"
#include <vector>
#include <memory>

enum class ShaderType;
class Mesh;
class Texture;
struct Vertex;

class Terrain
{
public:
    Terrain(float size, int subdivisions,ShaderType shaderType);
    ~Terrain();

    void Update();

    void SetVertexHeight(int x, int z, float height);
    void SetTexture(const std::vector<Texture>& textures);
    int GetNumberOfTriangles();

    Vec3<float> m_Rotation;
    Vec3<float> m_Translation;
    Vec3<float> m_Scale;
    ShaderType m_ShaderType;
private:
    std::vector<Vertex> GenerateVertices(float size, int subdivisions);
    std::vector<unsigned int> GenerateIndices(int subdivisions);
    
    std::unique_ptr<Mesh> m_Mesh;
    float m_Size;
    int m_Subdivisions;
};
