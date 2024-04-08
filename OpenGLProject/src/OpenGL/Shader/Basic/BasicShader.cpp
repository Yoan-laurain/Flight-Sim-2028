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

	const Camera& camera = *Application::Get()->GetCamera();
	SetUniformMat4fv("u_CamMatrix", camera.m_ViewProjectionMatrix);
}