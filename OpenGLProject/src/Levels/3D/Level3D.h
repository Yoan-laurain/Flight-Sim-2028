#pragma once

#include "../Level.h"

class PerlinNoiseModuleGPU;
class PerlinNoiseModuleCPU;
class TerrainModel;

class Level3D : public Level
{
	public:
		Level3D();
		~Level3D() override = default;
	
		void OnImGuiRender() override;
		void UpdateImGuiModulesParameters();
		void GenerateHeightMap();
		void OnTerrainSettingsChanged();

	private:

		Model* m_SkyBox;
	    Model* m_Plane;

		PerlinNoiseModuleGPU* tmp_perlinGPU;
		PerlinNoiseModuleCPU* tmp_perlinCPU;
};
