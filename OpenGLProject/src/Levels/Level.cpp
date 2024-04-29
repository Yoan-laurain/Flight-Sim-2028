#include "Level.h"
#include "../Model/Model.h"
#include "../Core/Application.h"
#include "../Managers/BatchRenderer/BatchRenderer.h"
#include "../Managers/ModelLoader/ModelLoader.h"

#include <imgui.h>

Level::Level() = default;

Level::~Level() = default;

void Level::OnRender() 
{
	Application::Get()->SetLinearColorSpace(true);
	
	m_CountDraws = Application::Get()->GetBatchRenderer()->Draw();

	Application::Get()->SetLinearColorSpace(false);
}

void Level::OnImGuiRender()
{

}

void Level::BeginPlay()
{
	UpdateTrianglesCount();
}

Model* Level::AddModel(const char* file, const ShaderType shaderType)
{
	ModelLoader loader(file, shaderType);
	
	m_Models.push_back( std::unique_ptr<Model>(loader.m_Model.release()) );
	return m_Models.back().get();
}

void Level::UpdateTrianglesCount()
{
	for (auto& shape : m_Models)
	{
		m_TriangleCount += shape.get()->GetNumberOfTriangles();
	}
}