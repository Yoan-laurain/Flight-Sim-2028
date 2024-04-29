#pragma once

#include "../Texture.h"

class CubeMapTexture : public Texture
{
	public:
		CubeMapTexture(const std::string* faces, unsigned int numFaces,const char* texType);
		~CubeMapTexture() override = default;

		/* Inherited via Texture */
		void Bind() const override;
		void Unbind() const override;
		/* Inherited via Texture */

	private:
		unsigned int m_NumFaces;
};