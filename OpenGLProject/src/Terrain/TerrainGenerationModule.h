#pragma once

#include <vector>

class TerrainGenerationModule
{
public:
    virtual ~TerrainGenerationModule() {}

    virtual void Process(std::vector<std::vector<float>>& heighmap) = 0;
};