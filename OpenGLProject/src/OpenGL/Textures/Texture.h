#pragma once

#include <GL/glew.h>
#include <string>

class Texture
{
	public:

		Texture(const char* image, const char* texType);
		Texture() = default; 
		virtual ~Texture() = default;

		virtual void Bind() const;
		virtual void Unbind() const;

		const char* GetType() const;

		GLuint m_Slot;
		std::string m_FilePath;
		int m_Index;

	private:
		void HandleFormat();
	
	protected:
		unsigned int m_ID;
	
		const char* m_Type;
	
		unsigned char* m_LocalBuffer;
		int m_Width, m_Height, m_BPP;
};