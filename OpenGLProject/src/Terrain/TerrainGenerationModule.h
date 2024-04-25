#pragma once

#include <memory>
#include <vector>

class TerrainGenerationModule
{
public:
    virtual ~TerrainGenerationModule() = default;
    virtual void Process(std::vector<float>& heighmap){};
};