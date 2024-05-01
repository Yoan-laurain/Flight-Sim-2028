#pragma once

#include "../../Library/coreMinimal.h"

#include <memory>
#include <vector>

class Texture;
struct Vertex;

class Mesh
{
	public:
	
		Mesh(std::vector<Vertex> vertices, const std::vector<unsigned int>& indices, std::vector<Texture> textures, const Mat4<float>& matrix);
		Mesh(const Mesh&);
		Mesh();
		virtual ~Mesh();

		Vec3<float> m_Transform;
		Quaternion<float> m_Rotation;
		Vec3<float> m_Scale;
		Mat4<float> m_Matrix;

		std::vector<Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;
	
		std::vector<Texture> m_Textures;
	
		std::vector<std::unique_ptr<Texture>> m_TextureSpecular;
		std::vector<std::unique_ptr<Texture>> m_TextureDiffuse;

	private:
		void FillTexturesArrays();

};