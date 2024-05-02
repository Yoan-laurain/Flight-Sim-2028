#pragma once

#include <vector>
#include <memory>

class Model;
class Mesh;
class Widget;

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
			m_models.push_back(std::unique_ptr<Model>(model));

			return model;
		}

	protected:
		std::vector<std::unique_ptr<Model>> m_models;
		std::unique_ptr<Widget> m_widget;
};