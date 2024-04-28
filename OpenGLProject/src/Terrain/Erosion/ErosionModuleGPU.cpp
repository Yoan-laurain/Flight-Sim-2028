#include "ErosionModuleGPU.h"
#include "Core/Application.h"
#include "Managers/ShaderManager/ShaderManager.h"
#include "OpenGL/Shader/Shader.h"
#include "OpenGL/ShaderStorageBuffer/ShaderStorageBufferObject.h"
#include "Terrain/TerrainGenerator.h"
#include "Config.h"
#include <random>

ErosionModuleGPU::ErosionModuleGPU()
{
    m_ErosionBrushIndicesBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_ErosionBrushWeightsBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_ErosionRandomIndexBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_MapBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_ErosionShader = Application::Get()->GetShaderManager()->GetShader(ShaderType::EROSION);
}

ErosionModuleGPU::~ErosionModuleGPU() = default;

void ErosionModuleGPU::Erode(std::vector<float>& heighmap)
{
    m_BrushIndexOffsets.clear();
    m_BrushWeights.clear();
    
    CreateBrushes();
    GenerateRandomIndicesForDropletPlacement();
    SetErosionShaderUniforms();
    
    m_MapBuffer->SetData(heighmap.data(), heighmap.size() * sizeof(float), 18);

    Application::Get()->GetShaderManager()->RunComputeShader(m_ErosionShader, m_NumErosionIterations,64);
    
    m_MapBuffer->GetData(heighmap.data(), heighmap.size() * sizeof(float), 0,18);
    
    UnBind();
}

void ErosionModuleGPU::Process(std::vector<float>& heighmap)
{
    Erode(heighmap);
}

void ErosionModuleGPU::CreateBrushes()
{
    float weightSum = 0;
    const int width = Application::Get()->GetTerrainGenerator()->GetSubdivisions();

    // Loop through each brush position
    for (int brushY = -ErosionBrushRadius; brushY <= ErosionBrushRadius; brushY++)
    {
        for (int brushX = -ErosionBrushRadius; brushX <= ErosionBrushRadius; brushX++)
        {
            // Calculate squared distance from center
            const float sqrDst = brushX * brushX + brushY * brushY;

            // Check if within erosion brush radius
            if (sqrDst < ErosionBrushRadius * ErosionBrushRadius)
            {
                // Calculate index offset and add to brush index offsets vector
                m_BrushIndexOffsets.push_back(brushY * width + brushX);
                
                // Calculate brush weight based on distance from center
                float brushWeight = 1 - sqrt(sqrDst) / ErosionBrushRadius;
                
                // Accumulate weight sum
                weightSum += brushWeight;
                
                // Add brush weight to brush weights vector
                m_BrushWeights.push_back(brushWeight);
            }
        }
    }

    for (float& m_BrushWeight : m_BrushWeights)
    {
        m_BrushWeight /= weightSum;
    }
    
    m_ErosionBrushIndicesBuffer->SetData(m_BrushIndexOffsets.data(), m_BrushIndexOffsets.size() * sizeof(int), 16);
    m_ErosionBrushWeightsBuffer->SetData(m_BrushWeights.data(), m_BrushWeights.size() * sizeof(float), 17);
}

void ErosionModuleGPU::GenerateRandomIndicesForDropletPlacement() const
{
    const int width = Application::Get()->GetTerrainGenerator()->GetSubdivisions();
    std::vector<int> randomIndices(m_NumErosionIterations);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_int_distribution distribution(ErosionBrushRadius, width + ErosionBrushRadius);
    
    // Generate random indices for droplet placement
    for (int i = 0; i < m_NumErosionIterations; i++)
    {
        // Generate random coordinates within the map bounds
        const int randomX = distribution(gen);
        const int randomY = distribution(gen);
        
        // Calculate index and store in randomIndices vector
        randomIndices[i] = randomY * width + randomX;
    }
    
    m_ErosionRandomIndexBuffer->SetData(randomIndices.data(), randomIndices.size() * sizeof(int), 15);
}

void ErosionModuleGPU::SetErosionShaderUniforms() const
{
    m_ErosionShader->Bind();

    m_ErosionShader->SetUniform1i("borderSize", ErosionBrushRadius);
    m_ErosionShader->SetUniform1i("brushLength", m_BrushIndexOffsets.size());
    m_ErosionShader->SetUniform1i("maxLifetime", m_MaxLifetime);
    m_ErosionShader->SetUniform1f("inertia", m_Inertia);
    m_ErosionShader->SetUniform1f("sedimentCapacityFactor", m_SedimentCapacityFactor);
    m_ErosionShader->SetUniform1f("minSedimentCapacity", m_MinSedimentCapacity);
    m_ErosionShader->SetUniform1f("depositSpeed", m_DepositSpeed);
    m_ErosionShader->SetUniform1f("erodeSpeed", m_ErodeSpeed);
    m_ErosionShader->SetUniform1f("evaporateSpeed", m_EvaporateSpeed);
    m_ErosionShader->SetUniform1f("gravity", m_Gravity);
    m_ErosionShader->SetUniform1f("startSpeed", m_StartSpeed);
    m_ErosionShader->SetUniform1f("startWater", m_StartWater);
    m_ErosionShader->SetUniform1i("mapSize", Application::Get()->GetTerrainGenerator()->m_BorderedMapSize);
}

void ErosionModuleGPU::UnBind() const
{
    m_MapBuffer->Unbind();
    m_ErosionBrushIndicesBuffer->Unbind();
    m_ErosionBrushWeightsBuffer->Unbind();
    m_ErosionRandomIndexBuffer->Unbind();
}