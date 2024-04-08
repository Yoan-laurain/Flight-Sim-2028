#include "CubeMapTexture.h"
#include "Core/Application.h"
#include "Managers/BatchRenderer.h"
#include "Vendor/stb_image/stb_image.h"
#include <iostream>

CubeMapTexture::CubeMapTexture(const std::string* faces, unsigned int numFaces,const char* texType) : Texture()
{
	m_NumFaces = numFaces;
	m_Type = texType;
	m_Slot = Application::Get()->GetBatchRenderer()->GetNextIndexToBindTextureTo();
	
	glGenTextures(1, &m_ID);
	CubeMapTexture::Bind();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < m_NumFaces; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_SRGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
}

void CubeMapTexture::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + m_Slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
}

void CubeMapTexture::Unbind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}