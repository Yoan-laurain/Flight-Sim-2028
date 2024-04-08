#include "VertexBuffer.h"
#include <GL/glew.h>

Vertex::Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& color, const glm::vec2& texUV)
    : m_Position( position )
    , m_Normal( normal )
    , m_Color( color )
    , m_TexUV( texUV )
    , m_IndexModel(0)
    , m_IndexDiffuse(-1)
    , m_IndexSpecular(-1)
{
}

Vertex::Vertex(const glm::vec3& position)
    : m_Position(position)
    , m_Normal(glm::vec3(0.0f))
    , m_Color(glm::vec3(1.0f))
    , m_TexUV(glm::vec2(0.0f))
    , m_IndexModel(0)
    , m_IndexDiffuse(-1)
    , m_IndexSpecular(-1)
{
}

VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices) : m_Vertices(vertices), m_ID(0)
{
    glGenBuffers(1, &m_ID);
    
    Bind();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_ID);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}