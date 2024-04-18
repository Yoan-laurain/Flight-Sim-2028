#include "VertexArray.h"
#include "../VertexBufferLayout/VertexBufferLayout.h"
#include "../VertexBuffer/VertexBuffer.h"

VertexArray::VertexArray() : m_ID(0)
{
	glGenVertexArrays(1, &m_ID);
	Bind();
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_ID);
	Unbind();
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const
{
	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.m_Count, element.m_Type, element.m_Normalized, layout.GetStride(), (const void*)offset);

		offset += element.m_Count * VertexBufferLayoutElement::GetSizeOfType(element.m_Type);
	}
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_ID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}