#include "ProceduralWorld.h"
#include "Mesh/SkyBox/SkyBox.h"
#include "Managers/ShaderManager/ShaderManager.h"
#include "Core/Application.h"
#include "Terrain/TerrainModel.h"
#include "Terrain/TerrainGenerator.h"
#include "Library/Math.h"
#include "Widget/ProceduralWorldWidget.h"

ProceduralWorld::ProceduralWorld()
{
	m_widget = std::make_unique<ProceduralWorldWidget>();
	
	AddModel<SkyBox,Model>(ShaderType::SKYBOX);

	CreateAirPlanes(20);
	
	Application::Get()->GetTerrainGenerator()->GenerateTerrain(300.f, 300.f,ShaderType::TERRAIN);
}

ProceduralWorld::~ProceduralWorld() = default;

void ProceduralWorld::DrawDebugsCubes()
{
	const int count = 7;
	const int spacing = 15;
    
	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < count; j++)
		{
			for (int k = 0; k < count; k++)
			{
				auto cube = AddModel("res/models/Cube/scene.gltf", ShaderType::BASIC);
				cube->SetTranslation(Vec3<float>( i * spacing, j * spacing, k * spacing));
			}
		}
	}
}

void ProceduralWorld::CreateAirPlanes(const unsigned count)
{
	for (unsigned i = 0; i < count; ++i)
	{
		Model* plane = AddModel(PLANE_FILE_PATH, ShaderType::BASIC);
		
		const float rndX = Math::rand(-50.f,50.f);
		const float rndY = Math::rand(25.f, 50.f);
		const float rndZ = Math::rand(-50.f,50.f);

		const float rotationX = Math::rand(-45.f, 45.f);
		const float rotationY = Math::rand(-180.f, 180.f);
		const float rotationZ = Math::rand(-45.f, 45.f);
		
		plane->SetTranslation(Vec3(rndX, rndY, rndZ));
		plane->SetRotation(Vec3(rotationX, rotationY, rotationZ));
	}
}