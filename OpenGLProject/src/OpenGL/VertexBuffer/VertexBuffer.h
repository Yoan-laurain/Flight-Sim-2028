#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex
{
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& color, const glm::vec2& texUV);
	explicit Vertex(const glm::vec3& position);

	glm::vec3 m_Position;
	glm::vec3 m_Normal;
	glm::vec3 m_Color;
	glm::vec2 m_TexUV;
	
	int m_IndexModel; 
	int m_IndexDiffuse; 
	int m_IndexSpecular; 
};

class VertexBuffer
{
	public:
		VertexBuffer( const std::vector<Vertex>& vertices );
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		std::vector<Vertex> m_Vertices;
	private:
		unsigned int m_ID;
};