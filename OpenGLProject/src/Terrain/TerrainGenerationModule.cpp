﻿#include "TerrainGenerationModule.h"

#include "TerrainGenerator.h"
#include "Core/Application.h"

TerrainGenerationModule::TerrainGenerationModule():m_isDirty(true),m_lastGeneratedHeighmap()
{
}

void TerrainGenerationModule::Generate(std::vector<float>& heighmap)
{
    if(m_isDirty)
    {
        Process(heighmap);
        m_lastGeneratedHeighmap = std::vector(heighmap);
        m_isDirty = false;
        return;
    }
    heighmap = m_lastGeneratedHeighmap;
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
    {
        double sum = 0;
        for(const auto& time : m_AverageGenerationTimeHistory)
        {
            sum += time;
        }
        return sum / m_AverageGenerationTimeHistory.size();
    }
}
