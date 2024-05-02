#include "TerrainShader.h"
#include "Core/Application.h"
#include "Camera/Camera.h"

#include <iostream>

TerrainShader::TerrainShader(const std::string& filepath)
    : Shader(filepath)
    , m_minTextureNormalThreshold(0.7f)
    , m_maxTextureNormalThreshold(0.9f)
    , m_snowThreshold(15.4f)
    , m_dirtThreshold(2.f)
    , m_snowBlendValue(0.5f)
    , m_dirtBlendValue(0.5f)
    
{
}

void TerrainShader::OnBeforeDraw()
{
    Bind();
	SetUniform1f("v_minTextureNormalThreshold",m_minTextureNormalThreshold);
    SetUniform1f("v_maxTextureNormalThreshold",m_maxTextureNormalThreshold);
    SetUniform1f("v_snowThreshold",m_snowThreshold);
    SetUniform1f("v_dirtThreshold",m_dirtThreshold);
    SetUniform1f("v_snowBlendValue",m_snowBlendValue);
    SetUniform1f("v_dirtBlendValue",m_dirtBlendValue);
    SetUniformMat4fv("u_CamMatrix", Application::Get()->GetCamera()->m_ViewProjectionMatrix);
}