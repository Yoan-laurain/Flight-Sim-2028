#include "PerlinNoiseModule.h"
#include <random>
#include "PerlinNoise.h"

PerlinNoiseModule::PerlinNoiseModule(float frequency):_frequency(frequency){}

void PerlinNoiseModule::Process(std::vector<std::vector<float>>& heighmap)
{
    std::random_device rd;
    PerlinNoise perlin = PerlinNoise(rd());

    int height = heighmap.size();
    if(height <=0)return;
    int width = heighmap[0].size();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float nx = x*_frequency;
            float ny = y*_frequency;

            heighmap[y][x] = perlin.noise(nx, ny);
        }
    }
}
