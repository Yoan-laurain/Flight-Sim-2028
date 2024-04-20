#pragma once

#include <memory>
#include <vector>

class TerrainGenerationModule
{
public:
    virtual ~TerrainGenerationModule() = default;
    virtual void Process(std::vector<std::vector<float>>& heighmap) = 0;
};