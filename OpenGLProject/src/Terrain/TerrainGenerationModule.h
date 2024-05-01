#pragma once

#include <vector>

class TerrainGenerationModule
{
    public:
        TerrainGenerationModule();
        virtual ~TerrainGenerationModule() = default;
        
        virtual void Process(std::vector<float>& heighmap) = 0;

        void Generate(std::vector<float>& heighmap);
    
        void SetDirty();
        bool IsDirty() const;
    
        double m_MaxGenerationTime = std::numeric_limits<float>::min();
        double m_MedGenerationTime = 0.f;
        double m_MinGenerationTime = std::numeric_limits<float>::max();
    
    private:
        bool m_isDirty;
        std::vector<float> m_lastGeneratedHeighmap;
};