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

		PerlinNoiseModuleGPU* m_PerlinGPU;
		PerlinNoiseModuleCPU* m_PerlinCPU;
		ErosionModuleGPU* m_ErosionGPU;
};
