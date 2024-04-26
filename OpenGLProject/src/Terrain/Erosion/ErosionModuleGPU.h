#pragma once

#include <vector>
#include <memory>

#include "Terrain/TerrainGenerationModule.h"

class ShaderStorageBufferObject;
class Shader;

class ErosionModuleGPU : public TerrainGenerationModule
{
    public:
        ErosionModuleGPU();
        ~ErosionModuleGPU() override;
    
        void Process(std::vector<float>& heighmap) override;
    
        void Erode(std::vector<float>& heighmap);
    
        int m_NumErosionIterations = 200000;
        int m_ErosionBrushRadius = 2;
        int m_MaxLifetime = 30;
        int m_MapSizeWithBorder;

        float m_SedimentCapacityFactor = 5.32f;
        float m_MinSedimentCapacity = .01f;
        float m_DepositSpeed = 0.3f;
        float m_ErodeSpeed = 0.2f;

        float m_EvaporateSpeed = .014f;
        float m_Gravity = 4;
        float m_StartSpeed = 1;
        float m_StartWater = 1;
        float m_Inertia = 0.04f;

    private:
        void CreateBrushes();
        void GenerateRandomIndicesForDropletPlacement() const;
        void SetErosionShaderUniforms();
    
        std::unique_ptr<ShaderStorageBufferObject> m_ErosionBrushIndicesBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_ErosionBrushWeightsBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_ErosionRandomIndexBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_MapBuffer;

        std::vector<int> m_BrushIndexOffsets;
        std::vector<float> m_BrushWeights;
        std::vector<float> m_HeightMap;
        Shader* m_ErosionShader;
};