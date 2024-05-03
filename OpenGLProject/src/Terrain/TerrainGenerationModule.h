#pragma once

#include <vector>

class TerrainGenerationModule
{
    public:
        TerrainGenerationModule();
        virtual ~TerrainGenerationModule() = default;
        
        virtual void Process(std::vector<float>& heightmap) = 0;

        void Generate(std::vector<float>& heightmap);
        void UpdateTimers(double generationTime);
    
        void SetDirty();
        bool IsDirty() const;
    
        double m_MaxGenerationTime;
        double m_MinGenerationTime;

        double GetAverageGenerationTime() const;
        std::vector<double> m_AverageGenerationTimeHistory;
    
    private:
        bool m_isDirty;
        std::vector<float> m_lastGeneratedHeighmap;
};