#pragma once

#include "../Library/coreMinimal.h"
#include <vector>
#include <memory>

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
	
		void SetRotation(Vec3<float> rotation);
		void SetTranslation(Vec3<float> translation);
		void SetScale(Vec3<float> scale);

		Vec3<float>& GetRotation();
		Vec3<float>& GetTranslation();
		Vec3<float>& GetScale();
		
		ShaderType m_ShaderType;
	
		std::vector<std::unique_ptr<Mesh>> m_Meshes;

	private:
	
		Vec3<float> m_Rotation;
		Vec3<float> m_Translation;
		Vec3<float> m_Scale;
		const char* m_File;
};