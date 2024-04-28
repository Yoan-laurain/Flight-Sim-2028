#include "PerlinNoiseModuleCPU.h"
#include "Terrain/PerlinNoise/PerlinNoise.h"
#include "Core/Application.h"
#include "Terrain/TerrainGenerator.h"
#include "Library/Vec2.h"
#include <random>

PerlinNoiseModuleCPU::PerlinNoiseModuleCPU() = default;

void PerlinNoiseModuleCPU::Process(std::vector<float>& heighmap)
{
    heighmap.clear();
    heighmap.resize(Application::Get()->GetTerrainGenerator()->m_BorderedMapSize * Application::Get()->GetTerrainGenerator()->m_BorderedMapSize);
    
    GenerateOffsets(-1000, 1000, m_Offsets);
    
    float minValue = std::numeric_limits<float>::max();
    float maxValue = std::numeric_limits<float>::min();
    const int mapSize = Application::Get()->GetTerrainGenerator()->m_BorderedMapSize;

    PerlinNoise perlin = PerlinNoise(m_Seed);
    
    for (int y = 0; y < mapSize; y++)
    {
        for (int x = 0; x < mapSize; x++)
        {
            float noiseValue = 0;
            float scale = 2;
            float weight = 1;
            for (int i = 0; i < m_NumOctaves; i++)
            {
                Vec2<float> p = Vec2(x / (float)mapSize, y / (float)mapSize) * scale + m_Offsets[i];
                noiseValue += perlin.noise(p.x, p.y) * weight;
                weight *= m_Persistence;
                scale *= m_Lacunarity;
            }
            heighmap[y * mapSize + x] = noiseValue;
            minValue = std::min(noiseValue, minValue);
            maxValue = std::max(noiseValue, maxValue);
        }
    }

    NormalizeHeightMap(heighmap, {static_cast<int>(minValue * m_FloatToIntMultiplier), static_cast<int>(maxValue * m_FloatToIntMultiplier)});
}