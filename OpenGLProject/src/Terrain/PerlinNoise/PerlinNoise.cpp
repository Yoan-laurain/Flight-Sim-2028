#include "PerlinNoise.h"
#include <algorithm>
#include <random>

PerlinNoise::PerlinNoise(const int seed)
    : grad(), perm()
{
    std::mt19937 gen(seed);
    std::uniform_real_distribution distr(-1.0f, 1.0f);

    for (int i = 0; i < 256; ++i)
    {
        grad[i] = { distr(gen), distr(gen) };
        perm[i] = i;
    }

    // Mélange des permutations
    std::shuffle(perm.begin(), perm.begin() + 256, gen);
    std::copy_n(perm.begin(), 256, perm.begin() + 256);
}

float PerlinNoise::noise(float x, float y) const
{
    const int X = static_cast<int>(std::floor(x)) & 255;
    const int Y = static_cast<int>(std::floor(y)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);

    float u = fade(x);
    float v = fade(y);

    const int A = perm[X] + Y;
    const int AA = perm[A];
    const int AB = perm[A + 1];
    const int B = perm[X + 1] + Y;
    const int BA = perm[B];
    const int BB = perm[B + 1];

    const float res = lerp(v,
                           lerp(u, dot(grad[AA], x, y), dot(grad[BA], x - 1, y)),
                           lerp(u, dot(grad[AB], x, y - 1), dot(grad[BB], x - 1, y - 1)));

    return res;
}

float PerlinNoise::fade(const float t) const
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(const float t, const float a, const float b) const
{
    return a + t * (b - a);
}

float PerlinNoise::dot(const std::array<float, 2>& g, const float x, const float y) const
{
    return g[0] * x + g[1] * y;
}