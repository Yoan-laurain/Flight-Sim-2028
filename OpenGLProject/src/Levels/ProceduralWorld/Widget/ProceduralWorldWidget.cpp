#include "ProceduralWorldWidget.h"
#include "Core/Application.h"
#include "Library/MyImGui/MyImGui.h"
#include "Managers/ShaderManager/ShaderManager.h"
#include "OpenGL/Shader/Terrain/TerrainShader.h"
#include "Terrain/TerrainGenerator.h"
#include "Terrain/Erosion/ErosionModuleGPU.h"
#include "Terrain/PerlinNoise/PerlinNoiseBaseModule.h"
#include <imgui.h>

ProceduralWorldWidget::ProceduralWorldWidget()
	: m_perlinModule(nullptr)
	, m_erosionGPU(nullptr)
{
	UpdateImGuiModulesParameters();
}

ProceduralWorldWidget::~ProceduralWorldWidget() = default;

void ProceduralWorldWidget::OnImGuiRender()
{
	ImGui::SetNextWindowSize(ImVec2(400, 450));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize);

	if (ImGui::BeginTabBar("Settings"))
	{
		CreateGeneralSettings();
		CreatePerlinSettings();
		CreateErosionSettings();
		CreateTerrainShaderSettings();
		ImGui::EndTabBar();
	}

	ImGui::End();

	CreateOverlay();
}

void ProceduralWorldWidget::UpdateImGuiModulesParameters()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	m_perlinModule = terrainGenerator->GetModule<PerlinNoiseBaseModule>();
	m_erosionGPU = terrainGenerator->GetModule<ErosionModuleGPU>(); 
}

void ProceduralWorldWidget::CreateGeneralSettings() const
{
	const TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	
	if (ImGui::BeginTabItem("General"))
	{
		ImGui::SeparatorText("Monitoring");
			
		ImGui::Text("Draws: %d", Application::Get()->m_CountDraws);
		ImGui::Text("Triangles: %d", Application::Get()->m_TriangleCount);
		ImGui::Text("Number of models: %d", Application::Get()->m_ModelCount);

		ImGui::SeparatorText("Monitoring - All Modules");

		ImGui::Text("Max : %.3f ms", terrainGenerator->GetMaxGenerationTime());
		ImGui::Text("Avg : %.3f ms", terrainGenerator->GetAverageGenerationTime());
		ImGui::Text("Min : %.3f ms", terrainGenerator->GetMinGenerationTime());
			
		ImGui::SeparatorText("Options");
			
		if (ImGui::Checkbox("Wireframe Mode", &Application::Get()->GetPolygonMode()))
		{
			Application::Get()->SetPolygonMode();
		}
		
		if (ImGui::Checkbox("VSync", &Application::Get()->GetVSync()))
		{
			Application::Get()->SetVSync(Application::Get()->GetVSync());
		}

		ImGui::EndTabItem();
	}
}

void ProceduralWorldWidget::CreatePerlinSettings()
{
	TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
	
	if (m_perlinModule && ImGui::BeginTabItem("HeightMap"))
	{
		ImGui::SeparatorText("CheckBoxes");
			
		ImGui::Checkbox("Randomize Seed", &m_perlinModule->randomizeSeed);

		if (ImGui::Checkbox("Generate with GPU",&terrainGenerator->m_GenerateGPU))
		{
			terrainGenerator->UpdateGenerationModules();
			UpdateImGuiModulesParameters();
		}

		ImGui::SeparatorText("Sliders");

		MyImGui::SliderFloat("Height", terrainGenerator->m_ElevationScale, 0.0f, 100.0f, [=](const float newValue) {
			terrainGenerator->m_ElevationScale = newValue;
			terrainGenerator->UpdateTerrain();
		});
		
		MyImGui::SliderInt("Octaves", m_perlinModule->GetNumOctaves(), 1, 8, [this,terrainGenerator](const int newValue) {
			m_perlinModule->SetNumOctaves(newValue);
			terrainGenerator->UpdateTerrain();
		});
		
		MyImGui::SliderFloat("Persistence", m_perlinModule->GetPersistance(), 0.0f, 2.0f, [this,terrainGenerator](const float newValue) {
			m_perlinModule->SetPersistance(newValue);
			terrainGenerator->UpdateTerrain();
		});
		
		MyImGui::SliderFloat("Lacunarity", m_perlinModule->GetLacunarity(), 0.0f, 4.0f, [this,terrainGenerator](const float newValue) {
			m_perlinModule->SetLacunarity(newValue);
			terrainGenerator->UpdateTerrain();
		});
		
		MyImGui::SliderFloat("Scale", m_perlinModule->GetScale(), 1.f, 100.f, [this,terrainGenerator](const float newValue) {
			m_perlinModule->SetScale(newValue);
			terrainGenerator->UpdateTerrain();
		});
		
		ImGui::SeparatorText("Monitoring");

		ImGui::Text("Max : %.3f ms", m_perlinModule->m_MaxGenerationTime);
		ImGui::Text("Avg : %.3f ms", m_perlinModule->GetAverageGenerationTime());
		ImGui::Text("Min : %.3f ms", m_perlinModule->m_MinGenerationTime);
			
		ImGui::EndTabItem();
	}
}

void ProceduralWorldWidget::CreateErosionSettings() const
{
	if (m_erosionGPU && ImGui::BeginTabItem("Erosion"))
	{
		TerrainGenerator* terrainGenerator = Application::Get()->GetTerrainGenerator();
		
		ImGui::SeparatorText("Sliders");
			
		MyImGui::SliderInt("Iterations", m_erosionGPU->GetNumErosionIterations(), 0, 250000, [this,terrainGenerator](const int newValue) {
			m_erosionGPU->SetNumErosionIterations(newValue);
			terrainGenerator->UpdateTerrain();
		});
			
		MyImGui::SliderFloat("Sediment Capacity Factor", m_erosionGPU->GetSedimentCapacityFactor(), 0.0f, 10.0f, [this,terrainGenerator](const float newValue) {
			m_erosionGPU->SetSedimentCapacityFactor(newValue);
			terrainGenerator->UpdateTerrain();
		});
			
		MyImGui::SliderFloat("Evaporate Speed", m_erosionGPU->GetEvaporateSpeed(), 0.01f, 1.0f, [this,terrainGenerator](const float newValue) {
			m_erosionGPU->SetEvaporateSpeed(newValue);
			terrainGenerator->UpdateTerrain();
		});
			
		MyImGui::SliderFloat("Inertia", m_erosionGPU->GetInertia(), 0.0f, 0.2f, [this,terrainGenerator](const float newValue) {
			m_erosionGPU->SetInertia(newValue);
			terrainGenerator->UpdateTerrain();
		});

		ImGui::SeparatorText("Monitoring");

		ImGui::Text("Max : %.3f ms", m_erosionGPU->m_MaxGenerationTime);
		ImGui::Text("Avg : %.3f ms", m_erosionGPU->GetAverageGenerationTime());
		ImGui::Text("Min : %.3f ms", m_erosionGPU->m_MinGenerationTime);
			
		ImGui::EndTabItem();
	}
}

void ProceduralWorldWidget::CreateTerrainShaderSettings()
{
	if (ImGui::BeginTabItem("Terrain Shader"))
	{
		auto* terrainShader = dynamic_cast<TerrainShader*>(Application::Get()->GetShaderManager()->GetShader(ShaderType::TERRAIN));

		ImGui::SeparatorText("Grass > Stone Thresholds");
		MyImGui::SliderFloat("Normal Max", terrainShader->m_MaxTextureNormalThreshold, 0.0f, 1.0f, [=](const float newValue) {
					terrainShader->m_MaxTextureNormalThreshold = newValue;
				});
		MyImGui::SliderFloat("Normal Min", terrainShader->m_MinTextureNormalThreshold, 0.0f, 1.0f, [=](const float newValue) {
					terrainShader->m_MinTextureNormalThreshold = newValue;
				});
		ImGui::SeparatorText("Snow");
		MyImGui::SliderFloat("Snow height", terrainShader->m_SnowThreshold, 0.0f, 20.0f, [=](const float newValue) {
					terrainShader->m_SnowThreshold = newValue;
				});
		MyImGui::SliderFloat("Snow Blend", terrainShader->m_SnowBlendValue, 0.0f, 1.0f, [=](const float newValue) {
					terrainShader->m_SnowBlendValue = newValue;
				});
		ImGui::SeparatorText("Low Dirt");
		MyImGui::SliderFloat("Dirt height", terrainShader->m_DirtThreshold, -10.0f, 10.0f, [=](const float newValue) {
					terrainShader->m_DirtThreshold = newValue;
				});
		MyImGui::SliderFloat("Dirt Blend", terrainShader->m_DirtBlendValue, 0.0f, 1.0f, [=](const float newValue) {
					terrainShader->m_DirtBlendValue = newValue;
				});

		ImGui::EndTabItem();
	}
}

void ProceduralWorldWidget::CreateOverlay() const
{
	const int widthOverlay = 200;
	ImGui::SetNextWindowPos(ImVec2(static_cast<float>(Application::Get()->m_WindowWidth - widthOverlay), 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
	
	ImGui::SetNextWindowSize(ImVec2(widthOverlay, 10));
	ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::Text("%.3f ms/frame (%.1f FPS)", static_cast<float>(1000.0f / ImGui::GetIO().Framerate), static_cast<float>(ImGui::GetIO().Framerate));
	ImGui::End();

	ImGui::PopStyleColor();
}