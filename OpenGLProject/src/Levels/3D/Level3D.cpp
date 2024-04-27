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
#include "Terrain/Erosion/ErosionModuleGPU.h"
#include <imgui.h>

Level3D::Level3D() : m_PerlinGPU(nullptr), m_PerlinCPU(nullptr), m_ErosionGPU(nullptr)
{
	AddModel<SkyBox,Model>(ShaderType::SKYBOX);
	AddModel("res/models/airplane/scene.gltf", ShaderType::BASIC);

	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	terrainGenerator->GenerateTerrain(300.f, 500.f,ShaderType::BASIC);
	terrainGenerator->GetTerrain()->SendDataRender();
	
	UpdateImGuiModulesParameters();
}

void Level3D::OnTerrainSettingsChanged()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	terrainGenerator->UpdateTerrain();
	
	Application::Get()->GetBatchRenderer()->UpdateVerticesDatas(terrainGenerator->GetTerrain(), ShaderType::BASIC);
}

void Level3D::OnImGuiRender() // TODO : Refactor
{
	Level::OnImGuiRender();

	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	if (ImGui::Checkbox("Generate with GPU",&terrainGenerator->m_GenerateGPU))
	{
		terrainGenerator->UpdateGenerationModules();
		UpdateImGuiModulesParameters();
		OnTerrainSettingsChanged();
	}
	
	MyImGui::SliderFloat("Height", terrainGenerator->m_ElevationScale, 0.0f, 100.0f, [=](const float newValue) {
				terrainGenerator->m_ElevationScale = newValue;
				OnTerrainSettingsChanged();
			});
	if(m_PerlinGPU)
	{
		// octaves
		MyImGui::SliderInt("Octaves", m_PerlinGPU->m_NumOctaves, 1, 8, [=](const int newValue) {
			m_PerlinGPU->m_NumOctaves = newValue;
			OnTerrainSettingsChanged();
		});

		// persistence
		MyImGui::SliderFloat("Persistence", m_PerlinGPU->m_Persistence, 0.0f, 2.0f, [=](const float newValue) {
			m_PerlinGPU->m_Persistence = newValue;
			OnTerrainSettingsChanged();
		});

		// lacunarity
		MyImGui::SliderFloat("Lacunarity", m_PerlinGPU->m_Lacunarity, 0.0f, 4.0f, [=](const float newValue) {
			m_PerlinGPU->m_Lacunarity = newValue;
			OnTerrainSettingsChanged();
		});
	
		// initial scale
		// MyImGui::SliderFloat("Scale", tmp_perlinGPU->m_InitialScale, 0.1f, 4.0f, [=](const float newValue) {
		// 	 tmp_perlinGPU->m_InitialScale = newValue;
		// 	 OnTerrainSettingsChanged();
		//  });
	}
	if(m_PerlinCPU)
	{
		MyImGui::SliderFloat("Frequency", m_PerlinCPU->m_Frequency, 0.05f, 1.0f, [=](const float newValue) {
			 m_PerlinCPU->m_Frequency = newValue;
			 OnTerrainSettingsChanged();
		 });
	}

	if (ImGui::Checkbox("Wireframe Mode", &Application::Get()->GetPolygoneMode()))
	{
		Application::Get()->SetPolygoneMode();
	}
	
	// --------------- EROSION ---------------

	// num iterations
	MyImGui::SliderInt("Iterations", m_ErosionGPU->m_NumErosionIterations, 0, 250000, [=](const int newValue) {
		m_ErosionGPU->m_NumErosionIterations = newValue;
		OnTerrainSettingsChanged();
	});
	
	// carry capacity
	MyImGui::SliderFloat("Sediment Capacity Factor", m_ErosionGPU->m_SedimentCapacityFactor, 0.0f, 10.0f, [=](const float newValue) {
		m_ErosionGPU->m_SedimentCapacityFactor = newValue;
		OnTerrainSettingsChanged();
	});
	
	// evaporation
	MyImGui::SliderFloat("Evaporate Speed", m_ErosionGPU->m_EvaporateSpeed, 0.01f, 1.0f, [=](const float newValue) {
		m_ErosionGPU->m_EvaporateSpeed = newValue;
		OnTerrainSettingsChanged();
	});
	
	//inertia
	MyImGui::SliderFloat("Inertia", m_ErosionGPU->m_Inertia, 0.0f, 0.2f, [=](const float newValue) {
		m_ErosionGPU->m_Inertia = newValue;
		OnTerrainSettingsChanged();
	});
}

void Level3D::UpdateImGuiModulesParameters()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	//TODO To remove
	m_PerlinGPU = terrainGenerator->GetModule<PerlinNoiseModuleGPU>();
	m_PerlinCPU = terrainGenerator->GetModule<PerlinNoiseModuleCPU>();
	m_ErosionGPU = terrainGenerator->GetModule<ErosionModuleGPU>();
}