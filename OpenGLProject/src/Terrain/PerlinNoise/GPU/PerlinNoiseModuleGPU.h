#pragma once

#include "Terrain/PerlinNoise/PerlinNoiseBaseModule.h"
#include "Library/Vec2.h"

#include <memory>
#include <vector>

class ShaderStorageBufferObject;
class Shader;

class PerlinNoiseModuleGPU : public PerlinNoiseBaseModule
{
    public:
        PerlinNoiseModuleGPU();
        ~PerlinNoiseModuleGPU() override;

        /* Inherited via TerrainGenerationModule */
        void Process(std::vector<float>& heighmap) override;
        /* Inherited via TerrainGenerationModule */
    
    private:
    
        void SetSSBODatas(const std::vector<float>& heighmap);
        void SetUniforms(std::vector<float>& heighmap) const;
        void Unbind() const;
    
        std::unique_ptr<ShaderStorageBufferObject> m_OffsetsBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_MapBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_MinMaxBuffer;


        std::vector<int> m_MinMaxHeight;

        Shader* m_HeightMapShader;
};