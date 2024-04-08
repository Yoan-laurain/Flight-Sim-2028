#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

class Texture;
struct Vertex;

class Mesh
{
	public:
	
		Mesh(std::vector<Vertex> vertices, const std::vector<unsigned int>& indices, std::vector<Texture> textures, const glm::mat4& matrix);
		Mesh(const Mesh&);
		Mesh();
		virtual ~Mesh();

		glm::vec3 m_Transform;
		glm::quat m_Rotation;
		glm::vec3 m_Scale;
		glm::mat4 m_Matrix;
		int m_TriangleCount;

		std::vector<Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;
	
		std::vector<Texture> m_Textures;
	
		std::unique_ptr<Texture> m_TextureSpecular;
		std::unique_ptr<Texture> m_TextureDiffuse;

	private:
		void FillTexturesArrays();

};