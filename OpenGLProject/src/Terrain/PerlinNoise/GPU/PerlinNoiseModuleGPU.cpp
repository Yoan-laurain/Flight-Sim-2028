#include "PerlinNoiseModuleGPU.h"
#include "Core/Application.h"
#include "Managers/ShaderManager/ShaderManager.h"
#include "OpenGL/Shader/Shader.h"
#include "OpenGL/ShaderStorageBuffer/ShaderStorageBufferObject.h"
#include "Terrain/TerrainGenerator.h"
#include <random>

PerlinNoiseModuleGPU::PerlinNoiseModuleGPU() 
{
    m_offsetsBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_mapBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_minMaxBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_heightMapShader = Application::Get()->GetShaderManager()->GetShader(ShaderType::HEIGHTMAP);
}

PerlinNoiseModuleGPU::~PerlinNoiseModuleGPU() = default;

void PerlinNoiseModuleGPU::Process(std::vector<float>& heightmap)
{
    heightmap.clear();
    const int size = Application::Get()->GetTerrainGenerator()->m_BorderedMapSize * Application::Get()->GetTerrainGenerator()->m_BorderedMapSize;
    heightmap.resize(size);  
    
    GenerateOffsets(-10000, 10000, m_Offsets);
    
    SetSSBODatas(heightmap);
    SetUniforms(heightmap);

    Application::Get()->GetShaderManager()->RunComputeShader(m_heightMapShader, static_cast<int>(heightmap.size()),64);
    
    m_mapBuffer->GetData(heightmap.data(), static_cast<int>(heightmap.size()) * sizeof(float), 0,1);
    m_minMaxBuffer->GetData(m_minMaxHeight.data(), static_cast<int>(m_minMaxHeight.size()) * sizeof(int), 0,2);
    
    Unbind();
    
    NormalizeHeightMap(heightmap, m_minMaxHeight);
}

void PerlinNoiseModuleGPU::SetSSBODatas(const std::vector<float>& heightmap)
{
    m_offsetsBuffer->SetData(m_Offsets.data(), static_cast<int>(m_Offsets.size()) * sizeof(Vec2<int>),3);
    m_mapBuffer->SetData(heightmap.data(), static_cast<int>(heightmap.size()) * sizeof(float),1);
    
    m_minMaxHeight = { m_floatToIntMultiplier * m_numOctaves, 0 };

    m_minMaxBuffer->SetData(m_minMaxHeight.data(), static_cast<int>(m_minMaxHeight.size()) * sizeof(int),2);
}

void PerlinNoiseModuleGPU::SetUniforms(const std::vector<float>& heightmap) const
{
    m_heightMapShader->Bind();
    m_heightMapShader->SetUniform1i("u_mapSize", Application::Get()->GetTerrainGenerator()->m_BorderedMapSize);
    m_heightMapShader->SetUniform1i("u_octaves", m_numOctaves);
    m_heightMapShader->SetUniform1i("u_floatToIntMultiplier", m_floatToIntMultiplier);
    m_heightMapShader->SetUniform1i("u_heightMapSize", static_cast<int>(heightmap.size()));
    m_heightMapShader->SetUniform1f("u_lacunarity", m_lacunarity);
    m_heightMapShader->SetUniform1f("u_persistence", m_persistence);
    m_heightMapShader->SetUniform1f("u_scaleFactor", m_initialScale);
}

void PerlinNoiseModuleGPU::Unbind() const
{
    m_mapBuffer->Unbind();
    m_minMaxBuffer->Unbind();
    m_offsetsBuffer->Unbind();
}