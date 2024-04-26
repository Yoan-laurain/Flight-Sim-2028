#pragma once

#include "Terrain/TerrainGenerationModule.h"
#include "Library/Vec2.h"

#include <memory>
#include <vector>

class ShaderStorageBufferObject;
class Shader;

class PerlinNoiseModuleGPU : public TerrainGenerationModule
{
    public:
        PerlinNoiseModuleGPU();
        ~PerlinNoiseModuleGPU() override;
    
        void Process(std::vector<float>& heighmap) override;
    
        // --------------- [ MODULABLES ] --------------- //

        int m_NumOctaves = 8;
        float m_Persistence = .52f;
        float m_Lacunarity = 1.66;
        float m_InitialScale = 2;

    private:

        void ScaleHeightmapValues(std::vector<float>& heightmap);
        void SetDatas(const std::vector<float>& heighmap);
        void SetUniforms(std::vector<float>& heighmap, int m_MapSizeWithBorder);
        void GenerateOffsets();
        void Unbind();
    
        std::unique_ptr<ShaderStorageBufferObject> m_OffsetsBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_MapBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_MinMaxBuffer;

        std::vector<Vec2<int>> m_Offsets;
        std::vector<int> m_MinMaxHeight;

        Shader* m_HeightMapShader;
        int m_FloatToIntMultiplier = 1000;
        bool randomizeSeed = false;
};
