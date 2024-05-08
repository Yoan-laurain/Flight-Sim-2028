#pragma once

#include "../Shader.h"

class SkyBoxShader : public Shader
{
	public:
		explicit SkyBoxShader(const std::string& filepath);
		~SkyBoxShader() override = default;

		/* Inherited via Shader */
		void OnBeforeDraw() override;
		void OnAfterDraw() override;
		/* Inherited via Shader */
};