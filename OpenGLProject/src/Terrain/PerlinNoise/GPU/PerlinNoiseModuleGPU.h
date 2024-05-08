#pragma once

#include "Terrain/PerlinNoise/PerlinNoiseBaseModule.h"

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
        void Process(std::vector<float>& heightmap) override;
        /* Inherited via TerrainGenerationModule */
    
    private:
    
        void SetSSBODatas(const std::vector<float>& heightmap);
        void SetUniforms(const std::vector<float>& heightmap) const;
        void Unbind() const;
    
        std::unique_ptr<ShaderStorageBufferObject> m_offsetsBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_mapBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_minMaxBuffer;
    
        std::vector<int> m_minMaxHeight;
        Shader* m_heightMapShader;
};