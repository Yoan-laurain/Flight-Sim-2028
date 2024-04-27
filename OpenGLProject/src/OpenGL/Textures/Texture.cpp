#include "Texture.h"
#include "Vendor/stb_image/stb_image.h"
#include "Core/Application.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include <stdexcept>

Texture::Texture(const char* image,const char* texType, const ShaderType shaderType)
	: m_FilePath( image )
	, m_Index(0)
	, m_Type(texType)
	, m_ID(0)
	, m_LocalBuffer(nullptr)
	, m_Width(0)
	, m_Height(0)
	, m_BPP(0)
{
	m_Slot = Application::Get()->GetBatchRenderer()->GetNextIndexToBindTextureTo(shaderType);
	
	//Flip the texture vertically
	stbi_set_flip_vertically_on_load(1);

	// Load the image  
	m_LocalBuffer = stbi_load(image, &m_Width, &m_Height, &m_BPP, 0);
	
	glGenTextures(1, &m_ID);
	Texture::Bind();
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // How the texture is going to be rendered when it's smaller than the original size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // How the texture is going to be rendered when it's bigger than the original size
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Stretch the texture on the x-axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Stretch the texture on the y-axis

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	HandleFormat();

	glGenerateMipmap(GL_TEXTURE_2D);
	
	// free the buffer if it is not null
	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);

	Texture::Unbind(); 
}

void Texture::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + m_Slot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Unbind() const
{ 
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::HandleFormat() const
{
	if (m_BPP == 4)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_SRGB_ALPHA,
			m_Width,
			m_Height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			m_LocalBuffer
		);
	else if (m_BPP == 3)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_SRGB,
			m_Width,
			m_Height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			m_LocalBuffer
		);
	else if (m_BPP == 1)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_SRGB,
			m_Width,
			m_Height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			m_LocalBuffer
		);
	else
		throw std::invalid_argument("Automatic Texture type recognition failed");
}