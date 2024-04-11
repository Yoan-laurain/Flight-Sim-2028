#include "Level3D.h"
#include "../../SkyBox/SkyBox.h"
#include "../../Managers/ShaderManager.h"

Level3D::Level3D() 
{
	m_SkyBox = AddModel<SkyBox,Model>(ShaderType::SKYBOX);
	
	auto cube = AddModel("res/models/Cube/scene.gltf", ShaderType::BASIC);
	cube->SetScale(Vec3<float>(2, 2, 2));
	cube->SetRotation(Vec3(45.f,10.f,10.f));
}

void Level3D::OnImGuiRender()
{
	Level::OnImGuiRender();

	// ImGui::Checkbox("Plane mode", &Application::Get()->GetCamera()->m_PlaneMode);
}