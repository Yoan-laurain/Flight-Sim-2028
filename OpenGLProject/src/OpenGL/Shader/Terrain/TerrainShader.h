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

        float m_minTextureNormalThreshold;
        float m_maxTextureNormalThreshold;
        float m_snowThreshold;
        float m_dirtThreshold;
        float m_snowBlendValue;
        float m_dirtBlendValue;
};