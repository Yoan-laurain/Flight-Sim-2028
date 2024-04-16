#include "Level3D.h"
#include "../../Model/SkyBox/SkyBox.h"
#include "../../Managers/ShaderManager/ShaderManager.h"

Level3D::Level3D() 
{
	m_SkyBox = AddModel<SkyBox,Model>(ShaderType::SKYBOX);
	AddTerrain(100.0f, 100.0f, 10,ShaderType::BASIC);
}

void Level3D::OnImGuiRender()
{
	Level::OnImGuiRender();

	// ImGui::Checkbox("Plane mode", &Application::Get()->GetCamera()->m_PlaneMode);
}