#pragma once

#include "Levels/Widget.h"

class PerlinNoiseBaseModule;
class ErosionModuleGPU;

class ProceduralWorldWidget : public Widget
{
    public:

        ProceduralWorldWidget();
        ~ProceduralWorldWidget() override;

        /* Inherited from Widget */
        void OnImGuiRender() override;
        /* Inherited from Widget */
    
        void UpdateImGuiModulesParameters();
        void CreateGeneralSettings() const;
        void CreatePerlinSettings();
        void CreateErosionSettings() const;
        void CreateTerrainShaderSettings();
        void CreateOverlay() const;

    private:
        PerlinNoiseBaseModule* m_perlinModule;
        ErosionModuleGPU* m_erosionGPU;
};
