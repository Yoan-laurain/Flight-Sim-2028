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
    m_erosionBrushIndicesBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_erosionBrushWeightsBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_erosionRandomIndexBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_mapBuffer = std::make_unique<ShaderStorageBufferObject>();
    m_erosionShader = Application::Get()->GetShaderManager()->GetShader(ShaderType::EROSION);
}

ErosionModuleGPU::~ErosionModuleGPU() = default;

void ErosionModuleGPU::Erode(std::vector<float>& heightmap)
{
    m_brushIndexOffsets.clear();
    m_brushWeights.clear();
    
    CreateBrushes();
    GenerateRandomIndicesForDropletPlacement();
    SetErosionShaderUniforms();
    
    m_mapBuffer->SetData(heightmap.data(), static_cast<int>(heightmap.size()) * sizeof(float), 1);

    Application::Get()->GetShaderManager()->RunComputeShader(m_erosionShader, m_numErosionIterations,64);
    
    m_mapBuffer->GetData(heightmap.data(), static_cast<int>(heightmap.size()) * sizeof(float), 0,1);
    
    UnBind();
}

int ErosionModuleGPU::GetNumErosionIterations() const
{
    return m_numErosionIterations;
}

float ErosionModuleGPU::GetSedimentCapacityFactor() const
{
    return m_sedimentCapacityFactor;
}

float ErosionModuleGPU::GetEvaporateSpeed() const
{
    return m_evaporateSpeed;
}

float ErosionModuleGPU::GetInertia() const
{
    return m_inertia;
}

void ErosionModuleGPU::SetNumErosionIterations(const int numIterations)
{
    if(m_numErosionIterations == numIterations)
    {
        return;
    }
    m_numErosionIterations = numIterations;
    SetDirty();
}

void ErosionModuleGPU::SetSedimentCapacityFactor(const float factor)
{
    if(m_sedimentCapacityFactor == factor)
    {
        return;
    }
    m_sedimentCapacityFactor = factor;
    SetDirty();
}

void ErosionModuleGPU::SetEvaporateSpeed(const float speed)
{
    if(m_evaporateSpeed == speed)
    {
        return;
    }
    m_evaporateSpeed = speed;
    SetDirty();
}

void ErosionModuleGPU::SetInertia(const float inertia)
{
    if(m_inertia == inertia)
    {
        return;
    }
    m_inertia = inertia;
    SetDirty();
}

void ErosionModuleGPU::Process(std::vector<float>& heightmap)
{
    Erode(heightmap);
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
            const float sqrDst = static_cast<float>(brushX) * static_cast<float>(brushX) + static_cast<float>(brushY) * static_cast<float>(brushY);

            // Check if within erosion brush radius
            if (sqrDst < ErosionBrushRadius * ErosionBrushRadius)
            {
                // Calculate index offset and add to brush index offsets vector
                m_brushIndexOffsets.push_back(brushY * width + brushX);
                
                // Calculate brush weight based on distance from center
                float brushWeight = 1 - sqrt(sqrDst) / ErosionBrushRadius;
                
                weightSum += brushWeight;
                
                m_brushWeights.push_back(brushWeight);
            }
        }
    }

    for (float& m_BrushWeight : m_brushWeights)
    {
        m_BrushWeight /= weightSum;
    }
    
    m_erosionBrushIndicesBuffer->SetData(m_brushIndexOffsets.data(),static_cast<int>(m_brushIndexOffsets.size()) * sizeof(int), 3);
    m_erosionBrushWeightsBuffer->SetData(m_brushWeights.data(),static_cast<int>(m_brushWeights.size()) * sizeof(float), 4);
}

void ErosionModuleGPU::GenerateRandomIndicesForDropletPlacement() const
{
    const int width = Application::Get()->GetTerrainGenerator()->GetSubdivisions();
    std::vector<int> randomIndices(m_numErosionIterations);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_int_distribution distribution(ErosionBrushRadius, width + ErosionBrushRadius);
    
    // Generate random indices for droplet placement
    for (int i = 0; i < m_numErosionIterations; i++)
    {
        // Generate random coordinates within the map bounds
        const int randomX = distribution(gen);
        const int randomY = distribution(gen);
        
        // Calculate index and store in randomIndices vector
        randomIndices[i] = randomY * width + randomX;
    }
    
    m_erosionRandomIndexBuffer->SetData(randomIndices.data(), static_cast<int>(randomIndices.size()) * sizeof(int), 2);
}

void ErosionModuleGPU::SetErosionShaderUniforms() const
{
    m_erosionShader->Bind();

    m_erosionShader->SetUniform1i("u_borderSize", ErosionBrushRadius);
    m_erosionShader->SetUniform1i("u_brushLength", static_cast<int>(m_brushIndexOffsets.size()));
    m_erosionShader->SetUniform1i("u_maxLifetime", m_maxLifetime);
    m_erosionShader->SetUniform1f("u_inertia", m_inertia);
    m_erosionShader->SetUniform1f("u_sedimentCapacityFactor", m_sedimentCapacityFactor);
    m_erosionShader->SetUniform1f("u_minSedimentCapacity", m_minSedimentCapacity);
    m_erosionShader->SetUniform1f("u_depositSpeed", m_depositSpeed);
    m_erosionShader->SetUniform1f("u_erodeSpeed", m_erodeSpeed);
    m_erosionShader->SetUniform1f("u_evaporateSpeed", m_evaporateSpeed);
    m_erosionShader->SetUniform1f("u_gravity", m_gravity);
    m_erosionShader->SetUniform1f("u_startSpeed", m_startSpeed);
    m_erosionShader->SetUniform1f("u_startWater", m_startWater);
    m_erosionShader->SetUniform1i("u_mapSize", Application::Get()->GetTerrainGenerator()->m_BorderedMapSize);
}

void ErosionModuleGPU::UnBind() const
{
    m_mapBuffer->Unbind();
    m_erosionBrushIndicesBuffer->Unbind();
    m_erosionBrushWeightsBuffer->Unbind();
    m_erosionRandomIndexBuffer->Unbind();
}