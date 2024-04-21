#include "VertexBuffer.h"
#include <GL/glew.h>

Vertex::Vertex(const Vec3<float>& position, const Vec3<float>& normal, const Vec3<float>& color, const Vec2<float>& texUV)
    : m_Position( position )
    , m_Normal( normal )
    , m_Color( color )
    , m_TexUV( texUV )
    , m_IndexModel(0)
    , m_IndexDiffuse(-1)
    , m_IndexSpecular(-1)
{
}

Vertex::Vertex(const Vec3<float>& position)
    : m_Position(position)
    , m_Normal(Vec3(0.0f))
    , m_Color(Vec3(1.0f))
    , m_TexUV(Vec2(0.0f))
    , m_IndexModel(0)
    , m_IndexDiffuse(-1)
    , m_IndexSpecular(-1)
{
}

void VertexBuffer::SetDatas(const std::vector<Vertex>& vertices)
{
    glGenBuffers(1, &m_ID);
    
    Bind();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices) : m_Vertices(vertices), m_ID(0)
{
    SetDatas(vertices);
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