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

const std::vector<VertexBufferLayoutElement>& VertexBufferLayout::GetElements() const
{
    return m_Elements;
}

unsigned VertexBufferLayout::GetStride() const
{
    return m_Stride;
}