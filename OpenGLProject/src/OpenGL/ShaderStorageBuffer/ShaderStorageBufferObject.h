#pragma once

class ShaderStorageBufferObject
{
    public:
        ShaderStorageBufferObject();
        ~ShaderStorageBufferObject();
    
        void Bind( unsigned index = 0) const;
        void Unbind();
    
        void SetData(const void* data, unsigned int size, unsigned index) const;
        void SetSubData(const void* data, unsigned int size, unsigned int offset) const;
    
        unsigned int m_RendererID;
};