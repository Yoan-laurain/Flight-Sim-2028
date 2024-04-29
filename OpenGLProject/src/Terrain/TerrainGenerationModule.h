#pragma once

#include <vector>

class TerrainGenerationModule
{
    public:
        virtual ~TerrainGenerationModule() = default;
        virtual void Process(std::vector<float>& heighmap){};

        double m_MaxGenerationTime = std::numeric_limits<float>::min();
        double m_MedGenerationTime = 0.f;
        double m_MinGenerationTime = std::numeric_limits<float>::max();
};