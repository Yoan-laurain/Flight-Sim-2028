#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

enum class ShaderType;

class Shader;
class Mesh;

class Model
{
	public:
		Model(const char* file, ShaderType shaderType);
		Model(const std::vector<Mesh*>& meshes, ShaderType shaderType);
		virtual ~Model();

		int GetNumberOfTriangles();

		/*
		 * Update all the meshes in the model
		 * Rotate, translate, and scale as the model values
		*/
		virtual void Update();
		void SendDataRender(); 
	
		void SetRotation(glm::vec3 rotation);
		void SetTranslation(glm::vec3 translation);
		void SetScale(glm::vec3 scale);

		glm::vec3& GetRotation();
		glm::vec3& GetTranslation();
		glm::vec3& GetScale();
		
		ShaderType m_ShaderType;
	
		std::vector<std::unique_ptr<Mesh>> m_Meshes;

	private:
	
		glm::vec3 m_Rotation;
		glm::vec3 m_Translation;
		glm::vec3 m_Scale;
		const char* m_File;
};