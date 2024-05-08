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
        void Process(std::vector<float>& heightmap) override;
        /* Inherited via TerrainGenerationModule */
    
        void Erode(std::vector<float>& heightmap);
    
        int GetNumErosionIterations() const;
        float GetSedimentCapacityFactor() const;
        float GetEvaporateSpeed() const;
        float GetInertia() const;
    
        void SetNumErosionIterations(int numIterations);
        void SetSedimentCapacityFactor(float factor);
        void SetEvaporateSpeed(float speed);
        void SetInertia(float inertia);

    private:
    
        int m_numErosionIterations = 200000;
        float m_sedimentCapacityFactor = 5.32f;
        float m_evaporateSpeed = .014f;
        float m_inertia = 0.04f;
    
        void CreateBrushes();
        void GenerateRandomIndicesForDropletPlacement() const;
        void SetErosionShaderUniforms() const;
        void UnBind() const;
    
        std::unique_ptr<ShaderStorageBufferObject> m_erosionBrushIndicesBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_erosionBrushWeightsBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_erosionRandomIndexBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_mapBuffer;

        std::vector<int> m_brushIndexOffsets;
        std::vector<float> m_brushWeights;
        std::vector<float> m_heightMap;
        Shader* m_erosionShader;
    
        int m_maxLifetime = 30;

        float m_minSedimentCapacity = .01f;
        float m_depositSpeed = 0.3f;
        float m_erodeSpeed = 0.2f;

        float m_gravity = 4;
        float m_startSpeed = 1;
        float m_startWater = 1;
};