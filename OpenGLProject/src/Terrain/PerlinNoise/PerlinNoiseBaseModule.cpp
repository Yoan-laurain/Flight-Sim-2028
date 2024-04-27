#include "PerlinNoiseBaseModule.h"
#include "Library/Math.h"
#include <random>

void PerlinNoiseBaseModule::NormalizeHeightMap(std::vector<float>& heightmap, const std::vector<int>& minMaxHeight) const
{
    const float minValue = static_cast<float>(minMaxHeight[0]) / m_FloatToIntMultiplier;
    const float maxValue = static_cast<float>(minMaxHeight[1]) / m_FloatToIntMultiplier;
    
    for (float& i : heightmap)
    {
        i = Math::InverseLerp(minValue, maxValue, i);
    }
}

void PerlinNoiseBaseModule::GenerateOffsets(int min, int max,std::vector<Vec2<int>>& offsets)
{
    m_Seed = randomizeSeed ? std::random_device()() : 0;
    std::mt19937 prng(m_Seed);
    std::uniform_int_distribution distribution(min, max);
    
    offsets = std::vector<Vec2<int>>(m_NumOctaves);
    
    for (int i = 0; i < m_NumOctaves; i++)
    {
        offsets[i] = Vec2(distribution(prng), distribution(prng));
    }
}