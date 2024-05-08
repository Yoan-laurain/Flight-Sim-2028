#include "Level.h"
#include "Model/Model.h"
#include "Core/Application.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include "Managers/ModelLoader/ModelLoader.h"
#include "Levels/Widget.h"

Level::Level() = default;

Level::~Level() = default;

void Level::OnRender() 
{
	Application::Get()->SetLinearColorSpace(true);
	
	Application::Get()->m_CountDraws = Application::Get()->GetBatchRenderer()->Draw();

	Application::Get()->SetLinearColorSpace(false);
}

void Level::OnImGuiRender()
{
	if ( m_widget )
	{
		m_widget->OnImGuiRender();
	}
}

void Level::BeginPlay() {}

Model* Level::AddModel(const char* file, const ShaderType shaderType)
{
	ModelLoader loader(file, shaderType);
	
	m_models.push_back( std::unique_ptr<Model>(loader.m_Model.release()) );
	return m_models.back().get();
}