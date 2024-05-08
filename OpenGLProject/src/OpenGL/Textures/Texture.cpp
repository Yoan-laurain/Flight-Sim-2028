#include "Texture.h"
#include "Vendor/stb_image/stb_image.h"
#include "Core/Application.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include <stdexcept>
#include <GL/glew.h>

Texture::Texture(const char* image,const char* texType, const ShaderType shaderType)
	: m_FilePath( image )
	, m_Index(0)
	, m_Type(texType)
	, m_id(0)
	, m_localBuffer(nullptr)
	, m_width(0)
	, m_height(0)
	, m_bpp(0)
{
	m_Slot = Application::Get()->GetBatchRenderer()->GetNextIndexToBindTextureTo(shaderType);
	
	//Flip the Texture vertically
	stbi_set_flip_vertically_on_load(1);

	// Load the image  
	m_localBuffer = stbi_load(image, &m_width, &m_height, &m_bpp, 0);
	
	glGenTextures(1, &m_id);
	Texture::Bind();
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // How the Texture is going to be rendered when it's smaller than the original size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // How the Texture is going to be rendered when it's bigger than the original size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Stretch the Texture on the x-axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Stretch the Texture on the y-axis
	
	// Configures the way the Texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	HandleFormat();

	glGenerateMipmap(GL_TEXTURE_2D);
	
	// free the buffer if it is not null
	if (m_localBuffer)
	{
		stbi_image_free(m_localBuffer);
	}
	
	Texture::Unbind(); 
}

void Texture::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + m_Slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::Bind(const int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::Unbind() const
{ 
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::HandleFormat() const
{
	switch(m_bpp)
	{
		case 4:
			glTexImage2D(GL_TEXTURE_2D,0,GL_SRGB_ALPHA,m_width,m_height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_localBuffer);
			break;
		case 3:
			glTexImage2D(GL_TEXTURE_2D,0,GL_SRGB,m_width,m_height,0,GL_RGB,GL_UNSIGNED_BYTE,m_localBuffer);
			break;
		case 1:
			glTexImage2D(GL_TEXTURE_2D,0,GL_SRGB,m_width,m_height,0,GL_RED,GL_UNSIGNED_BYTE,m_localBuffer);
			break;
		default:
			throw std::invalid_argument("Automatic Texture type recognition failed");
	}
}