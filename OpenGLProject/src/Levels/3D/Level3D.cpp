#include "Level3D.h"
#include "../../SkyBox/SkyBox.h"
#include "../../Managers/ShaderManager/ShaderManager.h"

Level3D::Level3D() 
{
	m_SkyBox = AddModel<SkyBox,Model>(ShaderType::SKYBOX);
	
	const int count = 10;
	const int spacing = 15;
    
	for (int i = 0; i < count; i++)
	{
		auto cube = AddModel("res/models/Cube/scene.gltf", ShaderType::BASIC);
		cube->SetTranslation(Vec3<float>( i * spacing, 0, 0));
	}
}

void Level3D::OnImGuiRender()
{
	Level::OnImGuiRender();

	// ImGui::Checkbox("Plane mode", &Application::Get()->GetCamera()->m_PlaneMode);
}