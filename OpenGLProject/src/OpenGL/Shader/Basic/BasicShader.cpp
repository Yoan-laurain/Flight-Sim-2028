#include "BasicShader.h"
#include "Core/Application.h"
#include "Camera/Camera.h"

#include <iostream>

BasicShader::BasicShader(const std::string& filepath) : Shader(filepath)
{
}

void BasicShader::OnBeforeDraw()
{
	Bind();
	
	SetUniformMat4fv("u_CamMatrix", Application::Get()->GetCamera()->m_ViewProjectionMatrix);
}