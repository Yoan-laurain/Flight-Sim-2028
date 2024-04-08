#include "Level3D.h"
#include "../../SkyBox/SkyBox.h"
#include "../../Managers/ShaderManager.h"

Level3D::Level3D() 
{
	m_SkyBox = AddModel<SkyBox,Model>(ShaderType::SKYBOX);
}

void Level3D::OnImGuiRender()
{
	Level::OnImGuiRender();

	// ImGui::Checkbox("Plane mode", &Application::Get()->GetCamera()->m_PlaneMode);
}