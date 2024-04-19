#pragma once

#include "../Level.h"
#include "Terrain/TerrainGenerator.h"

class Level3D : public Level
{
	public:
		Level3D();
		~Level3D() override = default;
	
		void OnImGuiRender() override;

	private:

		Model* m_SkyBox;
	    Model* m_Plane;
		TerrainGenerator m_generator;
};
