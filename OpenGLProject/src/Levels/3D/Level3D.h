#pragma once

#include "../Level.h"

class ErosionModuleGPU;
class PerlinNoiseModuleGPU;
class PerlinNoiseModuleCPU;

class Level3D : public Level
{
	public:
		Level3D();
		~Level3D() override = default;
	
		void OnImGuiRender() override;

	private:
		void UpdateImGuiModulesParameters();
		void OnTerrainSettingsChanged();

		Model* m_SkyBox;
	    Model* m_Plane;

		PerlinNoiseModuleGPU* tmp_perlinGPU;
		PerlinNoiseModuleCPU* tmp_perlinCPU;
		ErosionModuleGPU* tmp_erosionGPU;
};
