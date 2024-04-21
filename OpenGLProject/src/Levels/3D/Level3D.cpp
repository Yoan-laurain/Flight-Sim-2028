#include "Level3D.h"
#include "../../Model/SkyBox/SkyBox.h"
#include "../../Managers/ShaderManager/ShaderManager.h"
#include "Core/Application.h"
#include "Library/MyImGui/MyImGui.h"
#include "Terrain/TerrainModel.h"
#include "Managers/ModelLoader/ModelLoader.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include "Terrain/TerrainGenerator.h"
#include <imgui.h>

void Level3D::GenerateHeightMap()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	terrainGenerator->Init( m_Terrain->m_Width, m_Terrain->m_Depth);

	m_Terrain->SetHeight( terrainGenerator->m_HeightMap );
}

void Level3D::OnTerrainSettingsChanged()
{
	GenerateHeightMap();
	Application::Get()->GetBatchRenderer()->UpdateVerticesDatas(m_Terrain, ShaderType::BASIC);
}

Level3D::Level3D()  
{
	m_SkyBox = AddModel<SkyBox,Model>(ShaderType::SKYBOX);
	//m_Plane = AddModel("res/models/airplane/scene.gltf", ShaderType::BASIC);

	m_Terrain = dynamic_cast<TerrainModel*>(AddTerrain(255.0f, 255.0f, 50,ShaderType::BASIC));
	
	GenerateHeightMap();
	m_Terrain->ValidateTerrain();
}

void Level3D::OnImGuiRender()
{
	Level::OnImGuiRender();

	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	
	// octaves
	MyImGui::SliderInt("Octaves", Application::Get()->GetTerrainGenerator()->m_NumOctaves, 1, 8, [=](const int newValue) {
		terrainGenerator->m_NumOctaves = newValue;
		OnTerrainSettingsChanged();
	});

	// persistence
	MyImGui::SliderFloat("Persistence", Application::Get()->GetTerrainGenerator()->m_Persistence, 0.0f, 2.0f, [=](const float newValue) {
		terrainGenerator->m_Persistence = newValue;
		OnTerrainSettingsChanged();
	});

	// lacunarity
	MyImGui::SliderFloat("Lacunarity", Application::Get()->GetTerrainGenerator()->m_Lacunarity, 0.0f, 4.0f, [=](const float newValue) {
		terrainGenerator->m_Lacunarity = newValue;
		OnTerrainSettingsChanged();
	});
	
	// initial scale
	// MyImGui::SliderFloat("Scale", Application::Get()->GetTerrainGenerator()->m_InitialScale, 0.1f, 4.0f, [=](const float newValue) {
	// 	terrainGenerator->m_InitialScale = newValue;
	// 	OnTerrainSettingsChanged();
	// });
	
	MyImGui::SliderFloat("Height", Application::Get()->GetTerrainGenerator()->m_ElevationScale, 0.0f, 18.0f, [=](const float newValue) {
		terrainGenerator->m_ElevationScale = newValue;
		OnTerrainSettingsChanged();
	});

	if (ImGui::Checkbox("Wireframe Mode", &Application::Get()->GetPolygoneMode()))
	{
		Application::Get()->SetPolygoneMode();
	}
}