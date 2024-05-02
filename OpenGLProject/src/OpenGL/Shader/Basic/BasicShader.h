#pragma once

#include "../Shader.h"

class BasicShader : public Shader
{
	public:
		explicit BasicShader(const std::string& filepath);
		~BasicShader() override = default;

		/* inherited from Shader */
		void OnBeforeDraw() override;
		/* inherited from Shader */
};