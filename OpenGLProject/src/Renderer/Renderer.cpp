#include "Renderer.h"
#include "../OpenGL/VertexArray/VertexArray.h"
#include "../OpenGL/IndexBuffer/IndexBuffer.h"
#include "../OpenGL/Shader/Shader.h"
#include "../Config.h"

#include <iostream>

Renderer::Renderer() = default;

Renderer::~Renderer() = default;

void Renderer::Clear() const
{
    glClearColor(pow(0.07f,gamma), pow(0.13f,gamma), pow(0.17f,gamma), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();

	glDrawElements(GL_TRIANGLES, ib.m_Indices.size(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::HandleError(GLenum source, const GLenum type, GLuint id, const GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    // TODO : Handle performance messages
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION || type == GL_DEBUG_TYPE_PERFORMANCE)
        return;
    
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            return;
        default:
                break;
    }

    std::cout << std::endl;

    std::cout << "Severity: ";
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "LOW";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "HIGH";
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "NOTIFICATION";
            break;

        default:
            std::cout << "UNKNOWN";
            break;
    }
    
    std::cout << std::endl;

    std::cout << "GL Error: " << message << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;
}