#pragma once

#include <vector>
#include "Model/Model.h"
#include "OpenGL/Textures/Texture.h"

enum class ShaderType;

class TerrainModel : public Model
{
public:
    TerrainModel(float width, float depth, int subdivisions, ShaderType shaderType);
    virtual ~TerrainModel();

    void SetVertexHeight(int x, int z, float height);

private:
    float m_Width;
    float m_Depth;
    int m_Subdivisions;
};
