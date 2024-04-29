#include "Level3D.h"
#include "../../Model/SkyBox/SkyBox.h"
#include "../../Managers/ShaderManager/ShaderManager.h"
#include "Core/Application.h"
#include "Library/MyImGui/MyImGui.h"
#include "Terrain/TerrainModel.h"
#include "Managers/ModelLoader/ModelLoader.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include "Terrain/TerrainGenerator.h"
#include "Terrain/PerlinNoise/GPU/PerlinNoiseModuleGPU.h"
#include "Terrain/Erosion/ErosionModuleGPU.h"
#include <imgui.h>

Level3D::Level3D() : m_PerlinModule(nullptr), m_ErosionGPU(nullptr)
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

void Level3D::OnImGuiRender() 
{
	Level::OnImGuiRender();
	
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize);

	if (ImGui::BeginTabBar("Settings"))
	{
		CreateGeneralSettings();
		CreatePerlinSettings();
		CreateErosionSettings();

		ImGui::EndTabBar();
	}

	ImGui::End();

	CreateOverlay();
}

void Level3D::CreateGeneralSettings() const
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	
	if (ImGui::BeginTabItem("General"))
	{
		ImGui::SeparatorText("Monitoring");
			
		ImGui::Text("Draws: %d", m_CountDraws);
		ImGui::Text("Triangles: %d", m_TriangleCount);
		ImGui::Text("Number of models: %llu", m_Models.size());

		ImGui::SeparatorText("Monitoring - All Modules");

		ImGui::Text("Max : %.3f ms", terrainGenerator->GetMaxGenerationTime());
		ImGui::Text("Med : %.3f ms", terrainGenerator->GetMedGenerationTime());
		ImGui::Text("Min : %.3f ms", terrainGenerator->GetMinGenerationTime());
			
		ImGui::SeparatorText("Options");
			
		if (ImGui::Checkbox("Wireframe Mode", &Application::Get()->GetPolygoneMode()))
		{
			Application::Get()->SetPolygoneMode();
		}

		ImGui::EndTabItem();
	}
}

void Level3D::CreatePerlinSettings()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	
	if (m_PerlinModule && ImGui::BeginTabItem("HeightMap"))
	{
		ImGui::SeparatorText("CheckBoxes");
			
		ImGui::Checkbox("Randomize Seed", &m_PerlinModule->randomizeSeed);

		if (ImGui::Checkbox("Generate with GPU",&terrainGenerator->m_GenerateGPU))
		{
			terrainGenerator->UpdateGenerationModules();
			UpdateImGuiModulesParameters();
			OnTerrainSettingsChanged();
		}

		ImGui::SeparatorText("Sliders");

		MyImGui::SliderFloat("Height", terrainGenerator->m_ElevationScale, 0.0f, 100.0f, [=](const float newValue) {
			terrainGenerator->m_ElevationScale = newValue;
			OnTerrainSettingsChanged();
		});
		
		MyImGui::SliderInt("Octaves", m_PerlinModule->m_NumOctaves, 1, 8, [=](const int newValue) {
			m_PerlinModule->m_NumOctaves = newValue;
			OnTerrainSettingsChanged();
		});
		
		MyImGui::SliderFloat("Persistence", m_PerlinModule->m_Persistence, 0.0f, 2.0f, [=](const float newValue) {
			m_PerlinModule->m_Persistence = newValue;
			OnTerrainSettingsChanged();
		});
		
		MyImGui::SliderFloat("Lacunarity", m_PerlinModule->m_Lacunarity, 0.0f, 4.0f, [=](const float newValue) {
			m_PerlinModule->m_Lacunarity = newValue;
			OnTerrainSettingsChanged();
		});
		
		MyImGui::SliderInt("Scale", m_PerlinModule->m_Scale, 1, 100, [=](const int newValue) {
			m_PerlinModule->m_Scale = newValue;
			OnTerrainSettingsChanged();
		});

		ImGui::SeparatorText("Monitoring");

		ImGui::Text("Max : %.3f ms", m_PerlinModule->m_MaxGenerationTime);
		ImGui::Text("Med : %.3f ms", m_PerlinModule->m_MedGenerationTime);
		ImGui::Text("Min : %.3f ms", m_PerlinModule->m_MinGenerationTime);
			
		ImGui::EndTabItem();
	}
}

void Level3D::CreateErosionSettings()
{
	if (m_ErosionGPU && ImGui::BeginTabItem("Erosion"))
	{
		ImGui::SeparatorText("Sliders");
			
		MyImGui::SliderInt("Iterations", m_ErosionGPU->m_NumErosionIterations, 0, 250000, [=](const int newValue) {
			m_ErosionGPU->m_NumErosionIterations = newValue;
			OnTerrainSettingsChanged();
		});
			
		MyImGui::SliderFloat("Sediment Capacity Factor", m_ErosionGPU->m_SedimentCapacityFactor, 0.0f, 10.0f, [=](const float newValue) {
			m_ErosionGPU->m_SedimentCapacityFactor = newValue;
			OnTerrainSettingsChanged();
		});
			
		MyImGui::SliderFloat("Evaporate Speed", m_ErosionGPU->m_EvaporateSpeed, 0.01f, 1.0f, [=](const float newValue) {
			m_ErosionGPU->m_EvaporateSpeed = newValue;
			OnTerrainSettingsChanged();
		});
			
		MyImGui::SliderFloat("Inertia", m_ErosionGPU->m_Inertia, 0.0f, 0.2f, [=](const float newValue) {
			m_ErosionGPU->m_Inertia = newValue;
			OnTerrainSettingsChanged();
		});

		ImGui::SeparatorText("Monitoring");

		ImGui::Text("Max : %.3f ms", m_ErosionGPU->m_MaxGenerationTime);
		ImGui::Text("Med : %.3f ms", m_ErosionGPU->m_MedGenerationTime);
		ImGui::Text("Min : %.3f ms", m_ErosionGPU->m_MinGenerationTime);
			
		ImGui::EndTabItem();
	}
}

void Level3D::CreateOverlay() const
{
	const int widthOverlay = 200;
	ImGui::SetNextWindowPos(ImVec2(Application::Get()->m_WindowWidth - widthOverlay, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
	
	ImGui::SetNextWindowSize(ImVec2(widthOverlay, 10));
	ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::PopStyleColor();
}

void Level3D::UpdateImGuiModulesParameters()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	//TODO To remove
	m_PerlinModule = terrainGenerator->GetModule<PerlinNoiseBaseModule>();
	m_ErosionGPU = terrainGenerator->GetModule<ErosionModuleGPU>();
}