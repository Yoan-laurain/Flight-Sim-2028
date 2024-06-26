#include "Shader.h"
#include "Core/Application.h"
#include <GL/glew.h>
#include <sstream>
#include <fstream>
#include <iostream>

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
	Application::Get()->SetFaceCulling(true);
}

void Shader::OnAfterDraw()
{
	Application::Get()->SetFaceCulling(false);
}

void Shader::Bind() const
{
	glUseProgram(m_ID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetMaxImageUnit(ShaderProgramSource& source) const
{
	const int total_units = Application::Get()->GetMaxSlotForTextures();

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
	
	m_ID = source.ComputeSource.empty() ? CreateShader(source.VertexSource, source.FragmentSource) : CreateComputeShader(source.ComputeSource);
	Bind();
}

int Shader::GetUniformLocation(const std::string& name)
{
	// Check if the uniform location is already stored in the cache
	if (m_uniformLocationCache.contains(name))
		return m_uniformLocationCache[name];

	// If it's not stored in the cache, get the location of the uniform
	const int location = glGetUniformLocation(m_ID, name.c_str());
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

	// Store the uniform location in the cache
	m_uniformLocationCache[name] = location;

	// Return the uniform location
	return location;
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath) const
{
	// Open the file where the shaders are stored
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1, COMPUTE = 2
	};

	std::string line;
	std::stringstream ss[3];
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
			else if (line.find("compute") != std::string::npos)
				type = ShaderType::COMPUTE;
		}
		else
		{
			// If the line doesn't contain the word "#shader", add it to the stringstream
			ss[static_cast<int>(type)] << line << '\n';
		}
	}

	// Return the source code of the vertex and fragment shaders
	return { ss[0].str(), ss[1].str(), ss[2].str() };
}

void Shader::HandleLinkError(const int Program)
{
	// display error message if linking failed
	GLint linkStatus;
	glGetProgramiv(Program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) {
		// Linking failed, retrieve and display the error message
		GLint infoLogLength;
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<GLchar> infoLog(infoLogLength + 1); // Using vector for dynamic memory allocation
		glGetProgramInfoLog(Program, 512, NULL, infoLog.data());
		std::cerr << "Shader program linking error:\n" << infoLog.data() << std::endl;
	}
}

unsigned Shader::CreateComputeShader(const std::string& computeShader)
{
	const int computeProgram = glCreateProgram();
	const unsigned int cs = CompileShader(GL_COMPUTE_SHADER, computeShader);
	glAttachShader(computeProgram, cs);
	glLinkProgram(computeProgram);
	glValidateProgram(computeProgram);

	HandleLinkError(computeProgram);

	glDeleteShader(cs);

	return computeProgram;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	// Create a shader object and get its ID
	const unsigned int id = glCreateShader(type);

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
		const auto message = static_cast<char*>(alloca(length * sizeof(char)));

		// Get the error message
		glGetShaderInfoLog(id, length, &length, message);

		// Print the error message
		std::cout << "Failed to compile ";

		switch ( type )
		{
			case GL_VERTEX_SHADER:
				std::cout << "vertex shader!";
				break;
			case GL_FRAGMENT_SHADER:
				std::cout << "fragment shader!";
				break;
			case GL_COMPUTE_SHADER:
				std::cout << "compute shader!";
				break;
			default:
				break;
		}
		
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
	const int program = glCreateProgram();
	const unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	const unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Then we attach the shaders to the program
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	// Link the shaders together
	glLinkProgram(program);

	// Check if the linking was successful
	glValidateProgram(program);

	// display error message if linking failed
	HandleLinkError(program);

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
	glUniform1iv(GetUniformLocation(name), static_cast<int>(value.size()), value.data());
}

void Shader::SetUniformMat4fv(const std::string& name, const Mat4<float>& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}