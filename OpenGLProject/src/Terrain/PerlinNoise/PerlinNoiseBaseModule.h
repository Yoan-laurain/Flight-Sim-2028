#pragma once

#include "Library/Vec2.h"
#include "Terrain/TerrainGenerationModule.h"

class PerlinNoiseBaseModule : public TerrainGenerationModule
{
    public:
        virtual ~PerlinNoiseBaseModule() = default;

        void NormalizeHeightMap(std::vector<float>& heightmap, const std::vector<int>& minMaxHeight) const;
        void GenerateOffsets(int min, int max,std::vector<Vec2<int>>& offsets);

        float GetPersistance() const;
        void SetPersistance(float persistance);
        
        float GetLacunarity() const;
        void SetLacunarity(float lacunarity);

        float GetScale() const;
        void SetScale(float scale);

        float GetInitialScale() const;
        void SetInitialScale(float initialScale);

        int GetNumOctaves() const;
        void SetNumOctaves(int numOctaves);

        int GetFloatToIntMultiplier() const;
        void SetFloatToIntMultiplier(int floatToIntMultiplier);

        int GetSeed() const;
        void SetSeed(int seed);
        
        bool randomizeSeed = false;

        std::vector<Vec2<int>> m_Offsets;
    
    protected:
        float m_persistence = .52f;
        float m_lacunarity = 1.66f;
        float m_scale = 40;
        float m_initialScale = 2;
        int m_numOctaves = 8;
        int m_floatToIntMultiplier = 1000;
        int m_seed = 0;
};