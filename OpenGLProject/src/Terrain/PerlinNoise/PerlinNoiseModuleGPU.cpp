#include "PerlinNoiseModuleGPU.h"

#include <random>

#include "Core/Application.h"
#include "Library/Math.h"
#include "Library/Vec3.h"
#include "Managers/ShaderManager/ShaderManager.h"
#include "OpenGL/Shader/Shader.h"
#include "OpenGL/ShaderStorageBuffer/ShaderStorageBufferObject.h"

PerlinNoiseModuleGPU::PerlinNoiseModuleGPU()
{
    m_OffsetsBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_MapBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_MinMaxBuffer = std::make_unique<ShaderStorageBufferObject>();
}

PerlinNoiseModuleGPU::~PerlinNoiseModuleGPU() = default;

void PerlinNoiseModuleGPU::Process(std::vector<float>& heighmap)
{

    StartProcess(heighmap);
    
    Shader* heightMapShader = Application::Get()->GetShaderManager()->GetShader(ShaderType::HEIGHTMAP);
    Vec3 workGroupSize = { 1, 1, 1 };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize.x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize.y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize.z);

    int mapSize = heighmap.size();
    
    int workGroupCountX = static_cast<int>(ceil(mapSize / (float)workGroupSize.x));
    int workGroupCountY = static_cast<int>(ceil(1 / (float)workGroupSize.y));
    int workGroupCountZ = static_cast<int>(ceil(1 / (float)workGroupSize.y));
    
    glUseProgram(heightMapShader->m_ID);
    glDispatchCompute(workGroupCountX, workGroupCountY, workGroupCountZ);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    FnishProcess(heighmap);
    
}

void PerlinNoiseModuleGPU::StartProcess(std::vector<float>& heighmap)
{
    const int size = heighmap.size();
    
    const bool randomizeSeed = true;

    const int seed = randomizeSeed ? std::random_device()() : 0;
    std::mt19937 prng(seed);
    std::uniform_int_distribution distribution(-10000, 10000);

    std::vector<Vec2<int>> offsets(m_NumOctaves);
    for (int i = 0; i < m_NumOctaves; i++) {
        offsets[i] = Vec2(distribution(prng), distribution(prng));
    }
    
    m_OffsetsBuffer->SetData(offsets.data(), offsets.size() * sizeof(Vec2<int>),14);
    m_MapBuffer->SetData(heighmap.data(), heighmap.size() * sizeof(float),12);
    
    const int floatToIntMultiplier = 1000;

    std::vector minMaxHeight = { floatToIntMultiplier * m_NumOctaves, 0 };
    m_MinMaxBuffer->SetData(minMaxHeight.data(), minMaxHeight.size() * sizeof(int),13);

    Shader* heightMapShader = Application::Get()->GetShaderManager()->GetShader(ShaderType::HEIGHTMAP);
    heightMapShader->Bind();
    heightMapShader->SetUniform1i("mapSize", size);
    heightMapShader->SetUniform1i("octaves", m_NumOctaves);
    heightMapShader->SetUniform1i("floatToIntMultiplier", floatToIntMultiplier);
    heightMapShader->SetUniform1i("heightMapSize", static_cast<int>(size));
    heightMapShader->SetUniform1f("lacunarity", m_Lacunarity);
    heightMapShader->SetUniform1f("persistence", m_Persistence);
    heightMapShader->SetUniform1f("scaleFactor", m_InitialScale);
}

void PerlinNoiseModuleGPU::FnishProcess(std::vector<float>& heighmap)
{
    const int floatToIntMultiplier = 1000;
    std::vector minMaxHeight = { floatToIntMultiplier * m_NumOctaves, 0 };
    
    m_MapBuffer->GetData(heighmap.data(), heighmap.size() * sizeof(float), 0,12);
    m_MinMaxBuffer->GetData(minMaxHeight.data(), minMaxHeight.size() * sizeof(int), 0,13);
    
    m_MapBuffer->Unbind();
    m_MinMaxBuffer->Unbind();
    m_OffsetsBuffer->Unbind();
    
    const float minValue = static_cast<float>(minMaxHeight[0]) / static_cast<float>(floatToIntMultiplier);
    const float maxValue = static_cast<float>(minMaxHeight[1]) / static_cast<float>(floatToIntMultiplier);
    
    for (float& i : heighmap)
    {
        i = Math::InverseLerp(minValue, maxValue, i);
    }
}
