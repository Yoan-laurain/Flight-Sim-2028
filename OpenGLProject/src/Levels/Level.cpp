#include "Level.h"
#include "../Model/Model.h"
#include "../Core/Application.h"
#include "../Terrain/TerrainShape.h"
#include "../Managers/BatchRenderer.h"
#include "../Managers/ModelLoader.h"

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

Terrain* Level::AddTerrain(const float size, const int subdivisions, const ShaderType shaderType)
{
	Terrain* terrain = new Terrain(size, subdivisions, shaderType);
	m_Terrains.push_back(std::unique_ptr<Terrain>(terrain));
	return terrain;
}

void Level::UpdateTrianglesCount()
{
	// TODO : Clean this up
	for (auto& shape : m_Models)
	{
		m_TriangleCount += shape.get()->GetNumberOfTriangles();
	}
	
	for(auto& terrain : m_Terrains)
	{
		m_TriangleCount += terrain.get()->GetNumberOfTriangles();
	}
}