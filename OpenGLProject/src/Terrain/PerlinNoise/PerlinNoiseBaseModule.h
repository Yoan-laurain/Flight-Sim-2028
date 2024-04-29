#pragma once

#include "Library/Vec2.h"
#include "Terrain/TerrainGenerationModule.h"

class PerlinNoiseBaseModule : public TerrainGenerationModule
{
    public:
        virtual ~PerlinNoiseBaseModule() = default;

        void NormalizeHeightMap(std::vector<float>& heightmap, const std::vector<int>& minMaxHeight) const;
        void GenerateOffsets(int min, int max,std::vector<Vec2<int>>& offsets);

        float m_Persistence = .52f;
        float m_Lacunarity = 1.66f;
        float m_Scale = 20;
        float m_InitialScale = 2;
        int m_NumOctaves = 8;
        int m_FloatToIntMultiplier = 1000;
        int m_Seed = 0;
        bool randomizeSeed = false;

        std::vector<Vec2<int>> m_Offsets;
};