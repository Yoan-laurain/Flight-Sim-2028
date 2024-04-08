#pragma once

#include "../Shader.h"

class SkyBoxShader : public Shader
{
	public:
		SkyBoxShader(const std::string& filepath);
		~SkyBoxShader() override = default;
	
		void OnBeforeDraw() override;
	
		void OnAfterDraw() override;
};