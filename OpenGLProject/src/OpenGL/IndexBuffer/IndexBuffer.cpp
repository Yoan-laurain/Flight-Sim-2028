#include "IndexBuffer.h"
#include <GL/glew.h>

IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices) 
	: m_Indices(indices)
	, m_ID(0)
{
	glGenBuffers(1, &m_ID);
	
	SetDatas(indices);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_ID);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::SetDatas(const std::vector<unsigned>& indices)
{
	m_Indices = indices;
	
	Bind();
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}