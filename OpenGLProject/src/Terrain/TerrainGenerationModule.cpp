#include "TerrainGenerationModule.h"
#include "TerrainGenerator.h"
#include "Core/Application.h"

TerrainGenerationModule::TerrainGenerationModule()
    : m_MaxGenerationTime(std::numeric_limits<double>::min())
    , m_MinGenerationTime(std::numeric_limits<double>::max())
    , m_isDirty(true)
{
}

void TerrainGenerationModule::Generate(std::vector<float>& heightmap)
{
    if(m_isDirty)
    {
        Process(heightmap);
        m_lastGeneratedHeighmap = std::vector(heightmap);
        m_isDirty = false;
        return;
    }
    heightmap = m_lastGeneratedHeighmap;
}

void TerrainGenerationModule::UpdateTimers(const double generationTime)
{
    m_AverageGenerationTimeHistory.push_back(generationTime);
    m_MinGenerationTime = generationTime < m_MinGenerationTime ? generationTime : m_MinGenerationTime;
    m_MaxGenerationTime = generationTime > m_MaxGenerationTime ? generationTime : m_MaxGenerationTime;
}

void TerrainGenerationModule::SetDirty()
{
    m_isDirty = true;
}

bool TerrainGenerationModule::IsDirty() const
{
    return m_isDirty;
}

double TerrainGenerationModule::GetAverageGenerationTime() const
{
    double sum = 0;
    for(const auto& time : m_AverageGenerationTimeHistory)
    {
        sum += time;
    }
    return sum / static_cast<double>(m_AverageGenerationTimeHistory.size());
}