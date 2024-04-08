#include "SkyBoxShader.h"
#include "Core/Application.h"
#include "Camera/Camera.h"

#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>

SkyBoxShader::SkyBoxShader(const std::string& filepath) : Shader(filepath)
{
}

void SkyBoxShader::OnBeforeDraw()
{
    // Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
    glDepthFunc(GL_LEQUAL);

    Bind();

    const Camera& camera = *Application::Get()->GetCamera();

    const glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(camera.m_Position, camera.m_Position + camera.m_Rotation, camera.m_Up)));
    SetUniformMat4fv("u_View", view);
    
    SetUniformMat4fv("u_Projection", camera.m_ProjectionMatrix);
}

void SkyBoxShader::OnAfterDraw()
{
    // Switch back to the normal depth function
    glDepthFunc(GL_LESS);
}
