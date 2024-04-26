#include "PerlinNoiseModuleCPU.h"
#include "Library/Math.h"
#include "Terrain/PerlinNoise/PerlinNoise.h"
#include <random>

PerlinNoiseModuleCPU::PerlinNoiseModuleCPU(){}

void PerlinNoiseModuleCPU::Process(std::vector<float>& heighmap)
{
    std::random_device rd;
    PerlinNoise perlin = PerlinNoise(rd());
   
    int height =  sqrt(heighmap.size());
    if(height <=0)return;
    int width = height;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float nx = x*m_Frequency;
            float ny = y*m_Frequency;
            
            int index = y * width + x;
            heighmap[index] = perlin.noise(nx, ny);
        }
    }
}