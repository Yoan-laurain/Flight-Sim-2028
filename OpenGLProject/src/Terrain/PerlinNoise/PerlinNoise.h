#pragma once
#include <array>


class PerlinNoise {
public:
    PerlinNoise() = delete;
    PerlinNoise(int seed); 

    float noise(float x, float y) const;

private:
    float fade(float t) const;

    float lerp(float t, float a, float b) const;

    float dot(const std::array<float, 2>& g, float x, float y) const;

    std::array<std::array<float, 2>, 256> grad;
    std::array<int, 512> perm;
};