#pragma once

#include "Terrain/TerrainGenerationModule.h"

#include <vector>
#include <memory>

class ShaderStorageBufferObject;
class Shader;

class ErosionModuleGPU : public TerrainGenerationModule
{
    public:
        ErosionModuleGPU();
        ~ErosionModuleGPU() override;

        /* Inherited via TerrainGenerationModule */
        void Process(std::vector<float>& heighmap) override;
        /* Inherited via TerrainGenerationModule */
    
        void Erode(std::vector<float>& heighmap);


        int GetNumErosionIterations() const;
        float GetSedimentCapacityFactor() const;
        float GetEvaporateSpeed() const;
        float GetInertia() const;
    
        void SetNumErosionIterations(int numIterations);
        void SetSedimentCapacityFactor(float factor);
        void SetEvaporateSpeed(float speed);
        void SetInertia(float inertia);
        

    private:
    
        int m_NumErosionIterations = 200000;
        float m_SedimentCapacityFactor = 5.32f;
        float m_EvaporateSpeed = .014f;
        float m_Inertia = 0.04f;
    
        void CreateBrushes();
        void GenerateRandomIndicesForDropletPlacement() const;
        void SetErosionShaderUniforms() const;
        void UnBind() const;
    
        std::unique_ptr<ShaderStorageBufferObject> m_ErosionBrushIndicesBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_ErosionBrushWeightsBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_ErosionRandomIndexBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_MapBuffer;

        std::vector<int> m_BrushIndexOffsets;
        std::vector<float> m_BrushWeights;
        std::vector<float> m_HeightMap;
        Shader* m_ErosionShader;
    
        int m_MaxLifetime = 30;

        float m_MinSedimentCapacity = .01f;
        float m_DepositSpeed = 0.3f;
        float m_ErodeSpeed = 0.2f;

        float m_Gravity = 4;
        float m_StartSpeed = 1;
        float m_StartWater = 1;
};