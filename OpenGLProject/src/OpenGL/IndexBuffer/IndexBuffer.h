#pragma once

#include <vector>

class IndexBuffer
{
	public:

		IndexBuffer(const std::vector<unsigned int>& indices);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetDatas(const std::vector<unsigned int>& indices);

		std::vector<unsigned int> m_Indices;
	
	private:
		unsigned int m_id;
};