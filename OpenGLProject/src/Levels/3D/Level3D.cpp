#include "Level3D.h"
#include "../../Model/SkyBox/SkyBox.h"
#include "../../Managers/ShaderManager/ShaderManager.h"
#include "Library/MyImGui/MyImGui.h"

Level3D::Level3D() 
{
	m_SkyBox = AddModel<SkyBox,Model>(ShaderType::SKYBOX);
	m_Plane = AddModel("res/models/airplane/scene.gltf", ShaderType::BASIC);
}

void Level3D::OnImGuiRender()
{
	Level::OnImGuiRender();
	
	MyImGui::SliderFloat3("Translation : ",m_Plane->GetTranslation(), -10.0f, 10.0f, [this](const Vec3<float>& newPosition) {
		m_Plane->SetTranslation(newPosition);
	});
	
	MyImGui::SliderFloat3("Rotation : ",m_Plane->GetRotation(), -180.F, 180.f, [this](const Vec3<float>& newRotation) {
		m_Plane->SetRotation(newRotation);
	});
}