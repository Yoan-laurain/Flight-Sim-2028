#pragma once

#include "../Level.h"

class TerrainModel;

class Level3D : public Level
{
	public:
		Level3D();
		~Level3D() override = default;
	
		void OnImGuiRender() override;
		void GenerateHeightMap();
		void OnTerrainSettingsChanged();

	private:

		Model* m_SkyBox;
	    Model* m_Plane;
		TerrainModel* m_Terrain;
};
