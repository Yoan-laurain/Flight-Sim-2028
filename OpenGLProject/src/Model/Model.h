#pragma once

#include "../Library/coreMinimal.h"
#include <vector>
#include <memory>

enum class ShaderType;

class Mesh;

class Model
{
	public:
		Model(ShaderType shaderType);
		Model(const std::vector<Mesh*>& meshes, ShaderType shaderType);
		virtual ~Model();

		int GetNumberOfTriangles() const;

		/*
		 * Update all the meshes in the model
		 * Rotate, translate, and scale as the model values
		*/
		virtual void Update();
		void SendDataRender(); 
	
		void SetRotation(const Vec3<float>& rotation);
		void SetTranslation(const Vec3<float>& translation);
		void SetScale(const Vec3<float>& scale);

		Vec3<float> GetRotation() const;
		Vec3<float> GetTranslation() const;
		Vec3<float> GetScale() const;
		
		ShaderType m_ShaderType;
	
		std::vector<std::unique_ptr<Mesh>> m_Meshes;

	private:
		Vec3<float> m_Rotation;
		Vec3<float> m_Translation;
		Vec3<float> m_Scale;
};