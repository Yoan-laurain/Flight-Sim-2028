#include "SkyBoxShader.h"
#include "Core/Application.h"
#include "Camera/Camera.h"
#include "Library/Math.h"

#include <GL/glew.h>

SkyBoxShader::SkyBoxShader(const std::string& filepath) : Shader(filepath)
{
}

void SkyBoxShader::OnBeforeDraw()
{
    // Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
    glDepthFunc(GL_LEQUAL);

    Bind();

    const Camera& camera = *Application::Get()->GetCamera();
    const Mat3<float> viewMatrix = Math::mat3_cast(Math::lookAtM4(camera.m_Position, camera.m_Position + Math::radians(camera.m_Rotation), Math::radians(camera.m_Up)));
    
    SetUniformMat4fv("u_View", Mat4(viewMatrix));
    SetUniformMat4fv("u_Projection", camera.m_ProjectionMatrix);
}

void SkyBoxShader::OnAfterDraw()
{
    // Switch back to the normal depth function
    glDepthFunc(GL_LESS);
}