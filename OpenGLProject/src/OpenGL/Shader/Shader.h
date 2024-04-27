#pragma once 

#include "../../Library/coreMinimal.h"
#include <string>
#include <unordered_map>
#include <GL/glew.h>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
	std::string ComputeSource;
};

class Shader
{
	public:

		Shader( const std::string& filepath );
		virtual ~Shader();
		
		virtual void OnBeforeDraw();
		virtual void OnAfterDraw();

		void Bind() const;
		void Unbind() const;

		/* Set the maximum number of texture units available to the shader */
		void SetMaxImageUnit(ShaderProgramSource& source) const;

		void SetUniform1i(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform1iv(const std::string& name, const std::vector<int>& value);
		void SetUniformMat4fv(const std::string& name, const Mat4<float>& matrix);
		
		int GetUniformLocation(const std::string& name);

		unsigned int m_ID;
	
	private:
		
		std::unordered_map<std::string, int> m_UniformLocationCache;

		void SetShader(const std::string& filepath);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
		unsigned int CreateComputeShader(const std::string& computeShader);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		
		ShaderProgramSource ParseShader(const std::string& filepath) const;
		void HandleLinkError(GLuint Program);
};