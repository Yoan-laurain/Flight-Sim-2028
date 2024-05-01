#pragma once

#include "../Model/Model.h"

#include <vector>
#include <memory>

class Mesh;
enum class ShaderType;

class Level
{
	public:
		Level();
		virtual ~Level();

		virtual void OnRender();
		virtual void OnImGuiRender();
		virtual void BeginPlay();

		Model* AddModel(const char* file, ShaderType shaderType);

		template<typename T = Mesh, typename M = Model>
		Model* AddModel(ShaderType shaderType)
		{
			T* mesh = new T();
			M * model = new M({ mesh }, shaderType);
			m_Models.push_back(std::unique_ptr<Model>(model));

			return model;
		}

	protected:
		int m_CountDraws = 0;
		std::vector<std::unique_ptr<Model>> m_Models;
};