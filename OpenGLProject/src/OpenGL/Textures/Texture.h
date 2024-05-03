#pragma once

#include <string>

enum class ShaderType;

class Texture
{
public:

	Texture(const char* image, const char* texType, ShaderType shaderType);
	Texture() = default; 
	virtual ~Texture() = default;

	void Bind(int slot) const;
	virtual void Bind() const;
	virtual void Unbind() const;

	std::string m_FilePath;
	int m_Slot;
	int m_Index;
	const char* m_Type;

protected:
	unsigned int m_id;
	unsigned char* m_localBuffer;
	int m_width;
	int m_height;
	int m_bpp;
	
private:
	void HandleFormat() const;
};