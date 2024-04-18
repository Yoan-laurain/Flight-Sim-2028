#include "Shader.h"
#include <GL/glew.h>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Core/Application.h"

Shader::Shader(const std::string& filepath) : m_ID(0)
{
	SetShader(filepath);
}

Shader::~Shader()
{
	glDeleteProgram(m_ID);
}

void Shader::OnBeforeDraw()
{
}

void Shader::OnAfterDraw()
{
}

void Shader::Bind() const
{
	glUseProgram(m_ID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetMaxImageUnit(ShaderProgramSource& source)
{
	int total_units = Application::Get()->GetMaxSlotForTextures();

	std::string& fragment_source = source.FragmentSource;
	const std::string max_texture_units = "MAX_TEXTURE_UNITS X";
	const std::string max_texture_units_value = "MAX_TEXTURE_UNITS " + std::to_string(total_units);
	size_t pos = 0;
	
	while ((pos = fragment_source.find(max_texture_units, pos)) != std::string::npos)
	{
		fragment_source.replace(pos, max_texture_units.length(), max_texture_units_value);
		pos += max_texture_units_value.length();
	}
}

void Shader::SetShader(const std::string& filepath)
{
	ShaderProgramSource source = ParseShader(filepath);

	SetMaxImageUnit(source);
	
	m_ID = CreateShader(source.VertexSource, source.FragmentSource);
	Bind();
}

int Shader::GetUniformLocation(const std::string& name)
{
	// Check if the uniform location is already stored in the cache
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	// If it's not stored in the cache, get the location of the uniform
	const int location = glGetUniformLocation(m_ID, name.c_str());
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

	// Store the uniform location in the cache
	m_UniformLocationCache[name] = location;

	// Return the uniform location
	return location;
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	// Open the file where the shaders are stored
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	// Read the file line by line
	while (getline(stream, line))
	{
		// Check if the line contains the word "#shader"
		if (line.find("#shader") != std::string::npos)
		{
			// check if it's a vertex or fragment shader
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			// If the line doesn't contain the word "#shader", add it to the stringstream
			ss[(int)type] << line << '\n';
		}
	}

	// Return the source code of the vertex and fragment shaders
	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	// Create a shader object and get its ID
	unsigned int id = glCreateShader(type);

	const char* src = source.c_str();

	// Attach the source code to the shader
	glShaderSource(id, 1, &src, nullptr);

	// Since the GPU can't understand C++ code, we need to compile the shader to machine code
	glCompileShader(id);

	int result;

	// Check if the compilation was successful
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	// If the compilation failed, print the error message and delete the shader
	if (result == GL_FALSE)
	{
		int length;

		// Get the length of the error message
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		// Allocate memory on the stack to store the error message
		char* message = (char*)alloca(length * sizeof(char));

		// Get the error message
		glGetShaderInfoLog(id, length, &length, message);

		// Print the error message
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;

		// Delete the shader
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	// We need to create a program that will link the shaders together
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Then we attach the shaders to the program
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	// Link the shaders together
	glLinkProgram(program);

	// Check if the linking was successful
	glValidateProgram(program);

	// display error message if linking failed
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) {
		// Linking failed, retrieve and display the error message
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<GLchar> infoLog(infoLogLength + 1); // Using vector for dynamic memory allocation
		glGetProgramInfoLog(program, infoLogLength, NULL, infoLog.data());
		std::cerr << "Shader program linking error:\n" << infoLog.data() << std::endl;
	}

	// We can delete the shaders now that they are linked to the program
	glDeleteShader(vs);
	glDeleteShader(fs);

	// Return the program ID
	return program;
}

void Shader::SetUniform1i(const std::string& name, const int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, const float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform1iv(const std::string& name, const std::vector<int>& value)
{
	glUniform1iv(GetUniformLocation(name), value.size(), value.data());
}

void Shader::SetUniformMat4fv(const std::string& name, const Mat4<float>& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}