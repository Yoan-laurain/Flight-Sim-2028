#include "IndexBuffer.h"
#include <GL/glew.h>

IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices) 
	: m_Indices(indices)
	, m_ID(0)
	, m_NumberOfIndices(static_cast<unsigned int>(indices.size()))
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
	m_NumberOfIndices = static_cast<unsigned int>(indices.size());
	
	Bind();
	
	// Copy the data into the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

unsigned IndexBuffer::GetNumberOfIndices() const
{
	return m_NumberOfIndices;
}
