#pragma once

#include <vector>

/*
* The IndexBuffer class is used to store the index data of the vertices.
* The index data is used to determine the order in which the vertices are drawn.
* This helps to reduce the number of vertices that need to be stored in the vertex buffer.
*/
class IndexBuffer
{
	public:

		IndexBuffer(const std::vector<unsigned int>& indices);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetDatas(const std::vector<unsigned int>& indices);

		unsigned int GetNumberOfIndices() const;

		std::vector<unsigned int> m_Indices;
	private:
		unsigned int m_ID;
		unsigned int m_NumberOfIndices;
};