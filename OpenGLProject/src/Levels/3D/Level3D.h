#pragma once

#include "../Level.h"

class PerlinNoiseBaseModule;
class ErosionModuleGPU;

class Level3D : public Level
{
	public:
		Level3D();
		~Level3D() override = default;
	
		void OnImGuiRender() override;

	private:
		void UpdateImGuiModulesParameters();
		void OnTerrainSettingsChanged();

		PerlinNoiseBaseModule* m_PerlinModule;
		ErosionModuleGPU* m_ErosionGPU;
};
