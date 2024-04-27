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
    heighmap.resize(
        Application::Get()->GetTerrainGenerator()->m_BorderedMapSize * Application::Get()->GetTerrainGenerator()->
        m_BorderedMapSize);

    bool randomizeSeed = false;
    const int seed = randomizeSeed ? std::random_device()() : 0;
    std::mt19937 prng(seed);
    std::uniform_int_distribution distribution(-1000, 1000);

    PerlinNoise perlin = PerlinNoise(seed);

    int numOctaves = 8;
    std::vector<Vec2<int>> offsets(numOctaves);
    for (int i = 0; i < numOctaves; i++)
    {
        offsets[i] = {distribution(prng), distribution(prng)};
    }

    float minValue = std::numeric_limits<float>::max();
    float maxValue = std::numeric_limits<float>::min();

    int mapSize = Application::Get()->GetTerrainGenerator()->m_BorderedMapSize;
    float m_Persistence = .52f;
    float m_Lacunarity = 1.66f;

    for (int y = 0; y < mapSize; y++)
    {
        for (int x = 0; x < mapSize; x++)
        {
            float noiseValue = 0;
            float scale = 2;
            float weight = 1;
            for (int i = 0; i < numOctaves; i++)
            {
                Vec2<float> p = Vec2(x / (float)mapSize, y / (float)mapSize) * scale + offsets[i];
                noiseValue += perlin.noise(p.x, p.y) * weight;
                weight *= m_Persistence;
                scale *= m_Lacunarity;
            }
            heighmap[y * mapSize + x] = noiseValue;
            minValue = std::min(noiseValue, minValue);
            maxValue = std::max(noiseValue, maxValue);
        }
    }

    if (maxValue != minValue)
    {
        for (float& i : heighmap)
        {
            i = (i - minValue) / (maxValue - minValue);
        }
    }
}
