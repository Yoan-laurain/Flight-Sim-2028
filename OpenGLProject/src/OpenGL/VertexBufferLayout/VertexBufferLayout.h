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
		void Push<float>(unsigned int count);

		template<>
		void Push<unsigned int>(unsigned int count);

		template<>
		void Push<unsigned char>(unsigned int count);

		template<>
		void Push<int>(unsigned int count);

		const std::vector<VertexBufferLayoutElement>& GetElements() const;
		
		unsigned int GetStride() const;

	private:
		std::vector<VertexBufferLayoutElement> m_Elements;
		unsigned int m_Stride;
};