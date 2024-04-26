#include "Level3D.h"
#include "../../Model/SkyBox/SkyBox.h"
#include "../../Managers/ShaderManager/ShaderManager.h"
#include "Core/Application.h"
#include "Library/MyImGui/MyImGui.h"
#include "Terrain/TerrainModel.h"
#include "Managers/ModelLoader/ModelLoader.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include "Terrain/TerrainGenerator.h"
#include "Terrain/PerlinNoise/CPU/PerlinNoiseModuleCPU.h"
#include "Terrain/PerlinNoise/GPU/PerlinNoiseModuleGPU.h"
#include <imgui.h>

#include "Terrain/Erosion/ErosionModuleGPU.h"

void Level3D::OnTerrainSettingsChanged()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	terrainGenerator->CalculateTerrain();
	
	Application::Get()->GetBatchRenderer()->UpdateVerticesDatas(terrainGenerator->GetTerrain(), ShaderType::BASIC);
}

Level3D::Level3D()  
{
	m_SkyBox = AddModel<SkyBox,Model>(ShaderType::SKYBOX);
	//m_Plane = AddModel("res/models/airplane/scene.gltf", ShaderType::BASIC);

	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	terrainGenerator->GenerateTerrain(300.f, 500.f,ShaderType::BASIC);
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
	
	MyImGui::SliderFloat("Height", terrainGenerator->m_ElevationScale, 0.0f, 100.0f, [=](const float newValue) {
				terrainGenerator->m_ElevationScale = newValue;
				OnTerrainSettingsChanged();
			});
	if(tmp_perlinGPU)
	{
		// octaves
		MyImGui::SliderInt("Octaves", tmp_perlinGPU->m_NumOctaves, 1, 8, [=](const int newValue) {
			tmp_perlinGPU->m_NumOctaves = newValue;
			OnTerrainSettingsChanged();
		});

		// persistence
		MyImGui::SliderFloat("Persistence", tmp_perlinGPU->m_Persistence, 0.0f, 2.0f, [=](const float newValue) {
			tmp_perlinGPU->m_Persistence = newValue;
			OnTerrainSettingsChanged();
		});

		// lacunarity
		MyImGui::SliderFloat("Lacunarity", tmp_perlinGPU->m_Lacunarity, 0.0f, 4.0f, [=](const float newValue) {
			tmp_perlinGPU->m_Lacunarity = newValue;
			OnTerrainSettingsChanged();
		});
	
		// initial scale
		// MyImGui::SliderFloat("Scale", tmp_perlinGPU->m_InitialScale, 0.1f, 4.0f, [=](const float newValue) {
		// 	 tmp_perlinGPU->m_InitialScale = newValue;
		// 	 OnTerrainSettingsChanged();
		//  });
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
	
	// --------------- EROSION ---------------

	// num iterations
	MyImGui::SliderInt("Iterations", tmp_erosionGPU->m_NumErosionIterations, 0, 250000, [=](const int newValue) {
		tmp_erosionGPU->m_NumErosionIterations = newValue;
		OnTerrainSettingsChanged();
	});
	
	// carry capacity
	MyImGui::SliderFloat("Sediment Capacity Factor", tmp_erosionGPU->m_SedimentCapacityFactor, 0.0f, 10.0f, [=](const float newValue) {
		tmp_erosionGPU->m_SedimentCapacityFactor = newValue;
		OnTerrainSettingsChanged();
	});
	
	// evaporation
	MyImGui::SliderFloat("Evaporate Speed", tmp_erosionGPU->m_EvaporateSpeed, 0.01f, 1.0f, [=](const float newValue) {
		tmp_erosionGPU->m_EvaporateSpeed = newValue;
		OnTerrainSettingsChanged();
	});
	
	//inertia
	MyImGui::SliderFloat("Inertia", tmp_erosionGPU->m_Inertia, 0.0f, 0.2f, [=](const float newValue) {
		tmp_erosionGPU->m_Inertia = newValue;
		OnTerrainSettingsChanged();
	});
	
	//radius brush
	MyImGui::SliderInt("Brush Radius", tmp_erosionGPU->m_ErosionBrushRadius, 1, 5, [=](const int newValue) {
		tmp_erosionGPU->m_ErosionBrushRadius = newValue;
		OnTerrainSettingsChanged();
	});
}

void Level3D::UpdateImGuiModulesParameters()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	//TMP -> To remove
	tmp_perlinGPU = terrainGenerator->GetModule<PerlinNoiseModuleGPU>();
	tmp_perlinCPU = terrainGenerator->GetModule<PerlinNoiseModuleCPU>();
	tmp_erosionGPU = terrainGenerator->GetModule<ErosionModuleGPU>();
}
