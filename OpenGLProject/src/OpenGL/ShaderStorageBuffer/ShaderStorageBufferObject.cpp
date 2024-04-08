#include "ShaderStorageBufferObject.h"

#include <GL/glew.h>

ShaderStorageBufferObject::ShaderStorageBufferObject() : m_RendererID(0)
{
    glGenBuffers(1, &m_RendererID);
}

ShaderStorageBufferObject::~ShaderStorageBufferObject()
{
    glDeleteBuffers(1, &m_RendererID);
}

void ShaderStorageBufferObject::Bind(const unsigned index)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_RendererID);
}

void ShaderStorageBufferObject::Unbind()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBufferObject::SetData(const void* data, const unsigned size, const unsigned index)
{
    Bind(index);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
}

void ShaderStorageBufferObject::SetSubData(const void* data, const unsigned size, const unsigned offset)
{
    Bind();
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
}