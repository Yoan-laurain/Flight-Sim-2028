#include "Level3D.h"
#include "../../Model/SkyBox/SkyBox.h"
#include "../../Managers/ShaderManager/ShaderManager.h"
#include "Library/MyImGui/MyImGui.h"
#include "Terrain/TerrainModel.h"

Level3D::Level3D() 
{
	m_SkyBox = AddModel<SkyBox,Model>(ShaderType::SKYBOX);
	static_cast<TerrainModel*>(AddTerrain(100.0f, 100.0f, 100,ShaderType::BASIC))->ValidateTerrain();
	m_Plane = AddModel("res/models/airplane/scene.gltf", ShaderType::BASIC);
	m_Plane->SetRotation(Vec3(90.0f, 0.0f, 0.0f));
}

void Level3D::OnImGuiRender()
{
	Level::OnImGuiRender();
	
	MyImGui::SliderFloat3(m_Plane->GetTranslation(), -10.0f, 10.0f, [this](const Vec3<float>& newPosition) {
		m_Plane->SetTranslation(newPosition);
	});
}