#pragma once

#include <vector>

class TerrainGenerationModule
{
    public:
        virtual ~TerrainGenerationModule() = default;
        virtual void Process(std::vector<float>& heighmap){};

        double m_MaxGenerationTime = std::numeric_limits<float>::min();
        double m_MoyGenerationTime = 0.f;
        double m_MinGenerationTime = std::numeric_limits<float>::max();

        std::vector<double> m_MoyGenerationTimeHistory;

        double GetMoyGenerationTime() const
        {
            double sum = 0;
            for(const auto& time : m_MoyGenerationTimeHistory)
            {
                sum += time;
            }
            return sum / m_MoyGenerationTimeHistory.size();
        }
};