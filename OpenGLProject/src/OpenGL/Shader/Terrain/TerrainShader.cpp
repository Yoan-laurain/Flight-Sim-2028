﻿#include "TerrainShader.h"
#include "Core/Application.h"
#include "Camera/Camera.h"

#include <iostream>

TerrainShader::TerrainShader(const std::string& filepath)
    : Shader(filepath)
    , m_MinTextureNormalThreshold(0.7f)
    , m_MaxTextureNormalThreshold(0.9f)
    , m_SnowThreshold(15.4f)
    , m_DirtThreshold(2.f)
    , m_SnowBlendValue(0.5f)
    , m_DirtBlendValue(0.5f)
    
{
}

void TerrainShader::OnBeforeDraw()
{
    Bind();
	SetUniform1f("u_minTextureNormalThreshold",m_MinTextureNormalThreshold);
    SetUniform1f("u_maxTextureNormalThreshold",m_MaxTextureNormalThreshold);
    SetUniform1f("u_snowThreshold",m_SnowThreshold);
    SetUniform1f("u_dirtThreshold",m_DirtThreshold);
    SetUniform1f("u_snowBlendValue",m_SnowBlendValue);
    SetUniform1f("u_dirtBlendValue",m_DirtBlendValue);
    SetUniformMat4fv("u_CamMatrix", Application::Get()->GetCamera()->m_ViewProjectionMatrix);
}