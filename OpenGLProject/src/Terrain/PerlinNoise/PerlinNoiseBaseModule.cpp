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

float PerlinNoiseBaseModule::GetPersistance() const
{
    return m_Persistence;
}

void PerlinNoiseBaseModule::SetPersistance(float persistance)
{
    if(m_Persistence == persistance)
    {
        return;
    }
    m_Persistence = persistance;
    SetDirty();
}

float PerlinNoiseBaseModule::GetLacunarity() const
{
    return m_Lacunarity;
}

void PerlinNoiseBaseModule::SetLacunarity(float lacunarity)
{
    if(m_Lacunarity == lacunarity)
    {
        return;
    }
    m_Lacunarity = lacunarity;
    SetDirty();
}

float PerlinNoiseBaseModule::GetScale() const
{
    return m_Scale;
}

void PerlinNoiseBaseModule::SetScale(float scale)
{
    if(m_Scale == scale)
    {
        return;
    }
    m_Scale = scale;
    SetDirty();
}

float PerlinNoiseBaseModule::GetInitialScale() const
{
    return m_InitialScale;
}

void PerlinNoiseBaseModule::SetInitialScale(float initialScale)
{
    if(m_InitialScale == initialScale)
    {
        return;
    }
    m_InitialScale = initialScale;
    SetDirty();
}

int PerlinNoiseBaseModule::GetNumOctaves() const
{
    return m_NumOctaves;
}

void PerlinNoiseBaseModule::SetNumOctaves(int numOctaves)
{
    if(m_NumOctaves == numOctaves)
    {
        return;
    }
    m_NumOctaves = numOctaves;
    SetDirty();
}

int PerlinNoiseBaseModule::GetFloatToIntMultiplier() const
{
    return m_FloatToIntMultiplier;
}

void PerlinNoiseBaseModule::SetFloatToIntMultiplier(int floatToIntMultiplier)
{
    if(m_FloatToIntMultiplier == floatToIntMultiplier)
    {
        return;
    }
    m_FloatToIntMultiplier = floatToIntMultiplier;
    SetDirty();
}

int PerlinNoiseBaseModule::GetSeed() const
{
    return m_Seed;
}

void PerlinNoiseBaseModule::SetSeed(int seed)
{
    if(m_Seed == seed)
    {
        return;
    }
    m_Seed = seed;
    SetDirty();
}
