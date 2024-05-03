#pragma once

#include "../Shader.h"

class TerrainShader : public Shader
{
    public:
        explicit TerrainShader(const std::string& filepath);
        ~TerrainShader() override = default;
        
        /* inherited from Shader */
        void OnBeforeDraw() override;
        /* inherited from Shader */

        float m_MinTextureNormalThreshold;
        float m_MaxTextureNormalThreshold;
        float m_SnowThreshold;
        float m_DirtThreshold;
        float m_SnowBlendValue;
        float m_DirtBlendValue;
};