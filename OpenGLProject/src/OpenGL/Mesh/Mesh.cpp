#include "Mesh.h"
#include "OpenGL/Shader/Shader.h"
#include "OpenGL/IndexBuffer/IndexBuffer.h"
#include "OpenGL/Textures/Texture.h"
#include "OpenGL/VertexBuffer/VertexBuffer.h"
#include "Config.h"

Mesh::Mesh(std::vector<Vertex> vertices, const std::vector<unsigned int>& indices, std::vector<Texture> textures, const Mat4<float>& matrix)
	: m_Transform(Vec3(0.0f) )
	, m_Rotation( Quaternion(1.0f, 0.0f, 0.0f, 0.0f) )
	, m_Scale(Vec3(1.0f) )
	, m_Matrix(matrix)
	, m_TriangleCount(0)
	, m_Vertices(std::move(vertices))
	, m_Indices(indices)
	, m_Textures(std::move(textures))
{
	m_TriangleCount = static_cast<int>(indices.size()) / 3;
	
	FillTexturesArrays();
}

Mesh::Mesh(const Mesh&): m_TriangleCount(0)
{
}

Mesh::Mesh():m_TriangleCount(0){}

Mesh::~Mesh() = default;

void Mesh::FillTexturesArrays()
{
	for (auto& texture : m_Textures)
	{
		if (texture.GetType() == Specular)
		{
			m_TextureSpecular.emplace_back(std::make_unique<Texture>(texture));
		}
		else if (texture.GetType() == Diffuse)
		{
			m_TextureDiffuse.emplace_back(std::make_unique<Texture>(texture));
		}
	}
}