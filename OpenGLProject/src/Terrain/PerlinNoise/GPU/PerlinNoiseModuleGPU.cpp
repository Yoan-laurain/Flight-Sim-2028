#include "PerlinNoiseModuleGPU.h"
#include "Core/Application.h"
#include "Library/Math.h"
#include "Managers/ShaderManager/ShaderManager.h"
#include "OpenGL/Shader/Shader.h"
#include "OpenGL/ShaderStorageBuffer/ShaderStorageBufferObject.h"
#include "Terrain/TerrainGenerator.h"
#include "Config.h"
#include <random>

PerlinNoiseModuleGPU::PerlinNoiseModuleGPU() : m_MapSizeWithBorder(0)
{
    m_OffsetsBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_MapBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_MinMaxBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_HeightMapShader = Application::Get()->GetShaderManager()->GetShader(ShaderType::HEIGHTMAP);
}

PerlinNoiseModuleGPU::~PerlinNoiseModuleGPU() = default;

void PerlinNoiseModuleGPU::Process(std::vector<float>& heighmap)
{
    heighmap.clear();

    m_MapSizeWithBorder = Application::Get()->GetTerrainGenerator()->GetSubdivisions() + ErosionBrushRadius * 2;
    heighmap.resize(m_MapSizeWithBorder * m_MapSizeWithBorder);  
    
    GenerateOffsets();
    
    SetSSBODatas(heighmap);
    SetUniforms(heighmap);

    Application::Get()->GetShaderManager()->RunComputeShader(m_HeightMapShader, heighmap.size());
    
    m_MapBuffer->GetData(heighmap.data(), heighmap.size() * sizeof(float), 0,12);
    m_MinMaxBuffer->GetData(m_MinMaxHeight.data(), m_MinMaxHeight.size() * sizeof(int), 0,13);
    
    Unbind();
    
    ScaleHeightmapValues(heighmap);
}

void PerlinNoiseModuleGPU::SetSSBODatas(const std::vector<float>& heighmap)
{
    m_OffsetsBuffer->SetData(m_Offsets.data(), m_Offsets.size() * sizeof(Vec2<int>),14);
    m_MapBuffer->SetData(heighmap.data(), heighmap.size() * sizeof(float),12);
    
    m_MinMaxHeight = { m_FloatToIntMultiplier * m_NumOctaves, 0 };

    m_MinMaxBuffer->SetData(m_MinMaxHeight.data(), m_MinMaxHeight.size() * sizeof(int),13);
}

void PerlinNoiseModuleGPU::ScaleHeightmapValues(std::vector<float>& heightmap) const
{
    const float minValue = static_cast<float>(m_MinMaxHeight[0]) / m_FloatToIntMultiplier;
    const float maxValue = static_cast<float>(m_MinMaxHeight[1]) / m_FloatToIntMultiplier;
    
    for (float& i : heightmap)
    {
        i = Math::InverseLerp(minValue, maxValue, i);
    }
}

void PerlinNoiseModuleGPU::SetUniforms(std::vector<float>& heighmap) const
{
    m_HeightMapShader->Bind();
    m_HeightMapShader->SetUniform1i("mapSize", m_MapSizeWithBorder);
    m_HeightMapShader->SetUniform1i("octaves", m_NumOctaves);
    m_HeightMapShader->SetUniform1i("floatToIntMultiplier", m_FloatToIntMultiplier);
    m_HeightMapShader->SetUniform1i("heightMapSize", static_cast<int>(heighmap.size()));
    m_HeightMapShader->SetUniform1f("lacunarity", m_Lacunarity);
    m_HeightMapShader->SetUniform1f("persistence", m_Persistence);
    m_HeightMapShader->SetUniform1f("scaleFactor", m_InitialScale);
}

void PerlinNoiseModuleGPU::GenerateOffsets()
{
    const int seed = randomizeSeed ? std::random_device()() : 0;
    std::mt19937 prng(seed);
    std::uniform_int_distribution distribution(-10000, 10000);
    
    m_Offsets = std::vector<Vec2<int>>(m_NumOctaves);
    
    for (int i = 0; i < m_NumOctaves; i++)
    {
        m_Offsets[i] = Vec2(distribution(prng), distribution(prng));
    }
}

void PerlinNoiseModuleGPU::Unbind() const
{
    m_MapBuffer->Unbind();
    m_MinMaxBuffer->Unbind();
    m_OffsetsBuffer->Unbind();
}