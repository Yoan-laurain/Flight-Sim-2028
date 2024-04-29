#pragma once

#include <GL/glew.h>
#include <string>

enum class ShaderType;

class Texture
{
	public:

		Texture(const char* image, const char* texType, ShaderType shaderType);
		Texture() = default; 
		virtual ~Texture() = default;

		void Bind(int slot) const;
		virtual void Bind() const;
		virtual void Unbind() const;

		GLuint m_Slot;
		std::string m_FilePath;
		int m_Index;
		const char* m_Type;

	private:
		void HandleFormat() const;
	
	protected:
		unsigned int m_ID;
	
		unsigned char* m_LocalBuffer;
		int m_Width, m_Height, m_BPP;
};