#pragma once

#include "../Shader.h"

class TerrainShader : public Shader
{
public:
    TerrainShader(const std::string& filepath);
    ~TerrainShader() override = default;
    
    /* inherited from Shader */
    void OnBeforeDraw() override;
    /* inherited from Shader */

    float m_minTextureNormalThreshold = 0.7f;
    float m_maxTextureNormalThreshold = 0.9f;
    float m_snowThreshold = 8.f;
    float m_dirtThreshold = 2.f;
    float m_snowBlendValue = 0.5f;
    float m_dirtBlendValue = 0.5f;
};