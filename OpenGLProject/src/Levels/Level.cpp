#include "Level.h"
#include "../Model/Model.h"
#include "../Core/Application.h"
#include "../Terrain/TerrainShape.h"
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
	ImGui::Text("Draws: %d", m_CountDraws);
	ImGui::Text("Triangles: %d", m_TriangleCount);
	ImGui::Text("Number of models: %llu", m_Models.size());
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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
Model* Level::AddTerrain(float width, float depth, int subdivisions, ShaderType shaderType)
{
	TerrainModel* terrain = new TerrainModel(width, depth, subdivisions, shaderType);
	m_Models.push_back(std::unique_ptr<Model>(terrain));

	return terrain;
}


void Level::UpdateTrianglesCount()
{
	// TODO : Clean this up
	for (auto& shape : m_Models)
	{
		m_TriangleCount += shape.get()->GetNumberOfTriangles();
	}
	
}