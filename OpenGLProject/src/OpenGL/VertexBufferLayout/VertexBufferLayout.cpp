#include "VertexBufferLayout.h"
#include <cassert>

unsigned VertexBufferLayoutElement::GetSizeOfType(const unsigned type)
{
    switch (type)
    {
        case GL_INT:
        case GL_FLOAT:
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        default:
            break;
    }

    assert(false);
    return 0;
}

VertexBufferLayout::VertexBufferLayout() : m_Stride(0)
{
}

template <>
void VertexBufferLayout::Push<float>(const unsigned count)
{
    m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
    m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_FLOAT);
}

template <>
void VertexBufferLayout::Push<unsigned>(const unsigned count)
{
    m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
    m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_UNSIGNED_INT);
}

template <>
void VertexBufferLayout::Push<unsigned char>(const unsigned count)
{
    m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
    m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}

template <>
void VertexBufferLayout::Push<int>(const unsigned count)
{
    m_Elements.push_back({ GL_INT, count, GL_FALSE });
    m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_INT);
}

const std::vector<VertexBufferLayoutElement>& VertexBufferLayout::GetElements() const
{
    return m_Elements;
}

unsigned VertexBufferLayout::GetStride() const
{
    return m_Stride;
}