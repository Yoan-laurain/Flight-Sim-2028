#include "PerlinNoiseBaseModule.h"
#include "Library/Math.h"
#include <random>

void PerlinNoiseBaseModule::NormalizeHeightMap(std::vector<float>& heightmap, const std::vector<int>& minMaxHeight) const
{
    const float minValue = static_cast<float>(minMaxHeight[0]) / static_cast<float>(m_floatToIntMultiplier);
    const float maxValue = static_cast<float>(minMaxHeight[1]) / static_cast<float>(m_floatToIntMultiplier);
    
    for (float& i : heightmap)
    {
        i = Math::InverseLerp(minValue, maxValue, i);
    }
}

void PerlinNoiseBaseModule::GenerateOffsets(const int min, const int max,std::vector<Vec2<int>>& offsets)
{
    m_seed = randomizeSeed ? static_cast<int>(std::random_device()()) : 0;
    std::mt19937 prng(m_seed);
    std::uniform_int_distribution distribution(min, max);
    
    offsets = std::vector<Vec2<int>>(m_numOctaves);
    
    for (int i = 0; i < m_numOctaves; i++)
    {
        offsets[i] = Vec2(distribution(prng), distribution(prng));
    }
}

float PerlinNoiseBaseModule::GetPersistance() const
{
    return m_persistence;
}

void PerlinNoiseBaseModule::SetPersistance(const float persistance)
{
    if(m_persistence == persistance)
    {
        return;
    }
    m_persistence = persistance;
    SetDirty();
}

float PerlinNoiseBaseModule::GetLacunarity() const
{
    return m_lacunarity;
}

void PerlinNoiseBaseModule::SetLacunarity(const float lacunarity)
{
    if(m_lacunarity == lacunarity)
    {
        return;
    }
    m_lacunarity = lacunarity;
    SetDirty();
}

float PerlinNoiseBaseModule::GetScale() const
{
    return m_scale;
}

void PerlinNoiseBaseModule::SetScale(const float scale)
{
    if(m_scale == scale)
    {
        return;
    }
    m_scale = scale;
    SetDirty();
}

float PerlinNoiseBaseModule::GetInitialScale() const
{
    return m_initialScale;
}

void PerlinNoiseBaseModule::SetInitialScale(const float initialScale)
{
    if(m_initialScale == initialScale)
    {
        return;
    }
    m_initialScale = initialScale;
    SetDirty();
}

int PerlinNoiseBaseModule::GetNumOctaves() const
{
    return m_numOctaves;
}

void PerlinNoiseBaseModule::SetNumOctaves(const int numOctaves)
{
    if(m_numOctaves == numOctaves)
    {
        return;
    }
    m_numOctaves = numOctaves;
    SetDirty();
}

int PerlinNoiseBaseModule::GetFloatToIntMultiplier() const
{
    return m_floatToIntMultiplier;
}

void PerlinNoiseBaseModule::SetFloatToIntMultiplier(const int floatToIntMultiplier)
{
    if(m_floatToIntMultiplier == floatToIntMultiplier)
    {
        return;
    }
    m_floatToIntMultiplier = floatToIntMultiplier;
    SetDirty();
}

int PerlinNoiseBaseModule::GetSeed() const
{
    return m_seed;
}

void PerlinNoiseBaseModule::SetSeed(const int seed)
{
    if(m_seed == seed)
    {
        return;
    }
    m_seed = seed;
    SetDirty();
}