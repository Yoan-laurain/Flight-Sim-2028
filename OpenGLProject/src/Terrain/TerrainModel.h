#pragma once

#include <vector>
#include "Model/Model.h"
#include "OpenGL/Textures/Texture.h"

struct Vertex;
enum class ShaderType;

class TerrainModel : public Model
{
public:
    TerrainModel(float width, float depth, int subdivisions, ShaderType shaderType);
    virtual ~TerrainModel();

    void ValidateTerrain();
    void SetVertexHeight(int x, int z, float height) const;

private:
    float m_Width;
    float m_Depth;
    int m_Subdivisions;
};
