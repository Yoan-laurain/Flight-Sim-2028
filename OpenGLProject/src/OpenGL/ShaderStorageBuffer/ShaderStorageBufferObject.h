#pragma once

class ShaderStorageBufferObject
{
    public:
        ShaderStorageBufferObject();
        ~ShaderStorageBufferObject();
    
        void Bind( unsigned index = 0);
        void Unbind();
    
        void SetData(const void* data, unsigned int size, unsigned index);
        void SetSubData(const void* data, unsigned int size, unsigned int offset);
    
        unsigned int m_RendererID;
};