#include "PerlinNoiseModuleCPU.h"
#include "Terrain/PerlinNoise/PerlinNoise.h"
#include "Core/Application.h"
#include "Terrain/TerrainGenerator.h"
#include "Library/Vec2.h"
#include <random>

PerlinNoiseModuleCPU::PerlinNoiseModuleCPU() = default;

void PerlinNoiseModuleCPU::Process(std::vector<float>& heightmap)
{
    heightmap.clear();
    const int size = Application::Get()->GetTerrainGenerator()->m_BorderedMapSize;
    heightmap.resize(size * size);
    
    GenerateOffsets(-1000, 1000, m_Offsets);
    
    float minValue = std::numeric_limits<float>::max();
    float maxValue = std::numeric_limits<float>::min();
    const int mapSize = Application::Get()->GetTerrainGenerator()->m_BorderedMapSize;

    const PerlinNoise perlin = PerlinNoise(m_seed);
    
    for (int y = 0; y < mapSize; y++)
    {
        for (int x = 0; x < mapSize; x++)
        {
            float noiseValue = 0;
            float scale = 2;
            float weight = 1;
            for (int i = 0; i < m_numOctaves; i++)
            {
                Vec2<float> p = Vec2(static_cast<float>(x) / static_cast<float>(mapSize), static_cast<float>(y) / static_cast<float>(mapSize)) * scale + m_Offsets[i];
                noiseValue += perlin.noise(p.x, p.y) * weight;
                weight *= m_persistence;
                scale *= m_lacunarity;
            }
            heightmap[y * mapSize + x] = noiseValue;
            minValue = std::min(noiseValue, minValue);
            maxValue = std::max(noiseValue, maxValue);
        }
    }

    NormalizeHeightMap(heightmap, {static_cast<int>(minValue * static_cast<float>(m_floatToIntMultiplier)), static_cast<int>(maxValue * static_cast<float>(m_floatToIntMultiplier))});
}