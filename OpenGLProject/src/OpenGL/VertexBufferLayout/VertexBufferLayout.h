#pragma once

#include <vector>
#include <GL/glew.h>
#include <stdexcept>

struct VertexBufferLayoutElement
{
	unsigned int m_Type;
	unsigned int m_Count;
	unsigned char m_Normalized;

	static unsigned int GetSizeOfType(unsigned int type);
};

class VertexBufferLayout
{
	public:
		VertexBufferLayout();
		~VertexBufferLayout() = default;

		template<typename T>
		void Push(unsigned int count)
		{
			throw std::runtime_error("Unsupported type");
		}

		template<>
		void Push<float>(const unsigned int count)
		{
			m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
			m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_FLOAT);
		}

		template<>
		void Push<unsigned int>(const unsigned int count)
		{
			m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
			m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_UNSIGNED_INT);
		}

		template<>
		void Push<unsigned char>(const unsigned int count)
		{
			m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
			m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_UNSIGNED_BYTE);
		}
	
		template<>
		void Push<int>(const unsigned int count)
		{
			m_Elements.push_back({ GL_INT, count, GL_FALSE });
			m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_INT);
		}

		const std::vector<VertexBufferLayoutElement>& GetElements() const;
		
		unsigned int GetStride() const;

	private:
		std::vector<VertexBufferLayoutElement> m_Elements;
		unsigned int m_Stride;
};