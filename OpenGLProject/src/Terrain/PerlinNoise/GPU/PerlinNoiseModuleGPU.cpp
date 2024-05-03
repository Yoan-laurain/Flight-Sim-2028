#include "PerlinNoiseModuleGPU.h"
#include "Core/Application.h"
#include "Library/Math.h"
#include "Managers/ShaderManager/ShaderManager.h"
#include "OpenGL/Shader/Shader.h"
#include "OpenGL/ShaderStorageBuffer/ShaderStorageBufferObject.h"
#include "Terrain/TerrainGenerator.h"
#include "Config.h"
#include <random>

PerlinNoiseModuleGPU::PerlinNoiseModuleGPU() 
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
    heighmap.resize(Application::Get()->GetTerrainGenerator()->m_BorderedMapSize * Application::Get()->GetTerrainGenerator()->m_BorderedMapSize);  
    
    GenerateOffsets(-10000, 10000, m_Offsets);
    
    SetSSBODatas(heighmap);
    SetUniforms(heighmap);

    Application::Get()->GetShaderManager()->RunComputeShader(m_HeightMapShader, heighmap.size());
    
    m_MapBuffer->GetData(heighmap.data(), heighmap.size() * sizeof(float), 0,12);
    m_MinMaxBuffer->GetData(m_MinMaxHeight.data(), m_MinMaxHeight.size() * sizeof(int), 0,13);
    
    Unbind();
    
    NormalizeHeightMap(heighmap, m_MinMaxHeight);
}

void PerlinNoiseModuleGPU::SetSSBODatas(const std::vector<float>& heighmap)
{
    m_OffsetsBuffer->SetData(m_Offsets.data(), m_Offsets.size() * sizeof(Vec2<int>),3);
    m_MapBuffer->SetData(heighmap.data(), heighmap.size() * sizeof(float),1);
    
    m_MinMaxHeight = { m_FloatToIntMultiplier * m_NumOctaves, 0 };

    m_MinMaxBuffer->SetData(m_MinMaxHeight.data(), m_MinMaxHeight.size() * sizeof(int),2);
}

void PerlinNoiseModuleGPU::SetUniforms(std::vector<float>& heighmap) const
{
    m_HeightMapShader->Bind();
    m_HeightMapShader->SetUniform1i("mapSize", Application::Get()->GetTerrainGenerator()->m_BorderedMapSize);
    m_HeightMapShader->SetUniform1i("octaves", m_NumOctaves);
    m_HeightMapShader->SetUniform1i("floatToIntMultiplier", m_FloatToIntMultiplier);
    m_HeightMapShader->SetUniform1i("heightMapSize", static_cast<int>(heighmap.size()));
    m_HeightMapShader->SetUniform1f("lacunarity", m_Lacunarity);
    m_HeightMapShader->SetUniform1f("persistence", m_Persistence);
    m_HeightMapShader->SetUniform1f("scaleFactor", m_InitialScale);
}

void PerlinNoiseModuleGPU::Unbind() const
{
    m_MapBuffer->Unbind();
    m_MinMaxBuffer->Unbind();
    m_OffsetsBuffer->Unbind();
}