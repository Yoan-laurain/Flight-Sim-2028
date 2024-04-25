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

#include "Terrain/PerlinNoise/PerlinNoiseModuleCPU.h"
#include "Terrain/PerlinNoise/PerlinNoiseModuleGPU.h"

void Level3D::GenerateHeightMap()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	terrainGenerator->CalculateTerrain();
}

void Level3D::OnTerrainSettingsChanged()
{
	GenerateHeightMap();
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	
	Application::Get()->GetBatchRenderer()->UpdateVerticesDatas(terrainGenerator->GetTerrain(), ShaderType::BASIC);
	
}

Level3D::Level3D()  
{
	m_SkyBox = AddModel<SkyBox,Model>(ShaderType::SKYBOX);
	//m_Plane = AddModel("res/models/airplane/scene.gltf", ShaderType::BASIC);

	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	terrainGenerator->GenerateTerrain(100, 100, 50,ShaderType::BASIC);
	terrainGenerator->GetTerrain()->ValidateTerrain();

	UpdateImGuiModulesParameters();
	
}

void Level3D::OnImGuiRender()
{
	Level::OnImGuiRender();

	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	if (ImGui::Checkbox("Generate with GPU",&terrainGenerator->m_GenerateGPU))
	{
		terrainGenerator->UpdateGenerationMode();
		UpdateImGuiModulesParameters();
		OnTerrainSettingsChanged();
	}
	
	MyImGui::SliderFloat("Height", terrainGenerator->m_ElevationScale, 0.0f, 200.0f, [=](const float newValue) {
				terrainGenerator->m_ElevationScale = newValue;
				OnTerrainSettingsChanged();
			});
	if(tmp_perlinGPU)
	{
		// octaves
		MyImGui::SliderInt("Octaves", tmp_perlinGPU->m_NumOctaves, 1, 10, [=](const int newValue) {
			tmp_perlinGPU->m_NumOctaves = newValue;
			OnTerrainSettingsChanged();
		});

		// persistence
		MyImGui::SliderFloat("Persistence", tmp_perlinGPU->m_Persistence, 0.0f, 2.0f, [=](const float newValue) {
			tmp_perlinGPU->m_Persistence = newValue;
			OnTerrainSettingsChanged();
		});

		// lacunarity
		MyImGui::SliderFloat("Lacunarity", tmp_perlinGPU->m_Lacunarity, 0.0f, 10.0f, [=](const float newValue) {
			tmp_perlinGPU->m_Lacunarity = newValue;
			OnTerrainSettingsChanged();
		});
	
		// initial scale
		MyImGui::SliderFloat("Scale", tmp_perlinGPU->m_InitialScale, 0.1f, 50.0f, [=](const float newValue) {
			 tmp_perlinGPU->m_InitialScale = newValue;
			 OnTerrainSettingsChanged();
		 });
	}
	if(tmp_perlinCPU)
	{
		MyImGui::SliderFloat("Frequency", tmp_perlinCPU->m_Frequency, 0.05f, 1.0f, [=](const float newValue) {
			 tmp_perlinCPU->m_Frequency = newValue;
			 OnTerrainSettingsChanged();
		 });
	}
	
	
	

	if (ImGui::Checkbox("Wireframe Mode", &Application::Get()->GetPolygoneMode()))
	{
		Application::Get()->SetPolygoneMode();
	}
}

void Level3D::UpdateImGuiModulesParameters()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	//TMP -> To remove
	tmp_perlinGPU = terrainGenerator->GetModule<PerlinNoiseModuleGPU>();
	tmp_perlinCPU = terrainGenerator->GetModule<PerlinNoiseModuleCPU>();
}
