#pragma once

class ShaderStorageBufferObject
{
    public:
        ShaderStorageBufferObject();
        ~ShaderStorageBufferObject();
        
        void Bind( unsigned index = 0) const;
        void Unbind();

        void GetData(void* data, unsigned int size, unsigned int offset, unsigned index) const;
        void SetData(const void* data, unsigned int size, unsigned index);
        void SetSubData(const void* data, unsigned int size, unsigned int offset, unsigned index);

        unsigned int m_RendererID;
};