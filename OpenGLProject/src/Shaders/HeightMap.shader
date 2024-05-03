#shader compute
#version 460 core

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) buffer heightMapBuffer { float Data[]; } heightMap;
layout(std430, binding = 2) buffer minMaxBuffer    { int Data[];   } minMax;
layout(std430, binding = 3) buffer offsetsBuffer   { ivec2 Data[]; } offsets;

uniform int u_floatToIntMultiplier;
uniform int u_mapSize;
uniform int u_octaves;
uniform float u_persistence;
uniform float u_lacunarity;
uniform float u_scaleFactor;
uniform int u_heightMapSize;

vec2 mod289(vec2 x) { return x - floor(x / 289.0) * 289.0; }

vec3 mod289(vec3 x) {  return x - floor(x / 289.0) * 289.0; }

vec3 permute(vec3 x) {  return mod289((x * 34.0 + 1.0) * x); }

const vec4 C = vec4(0.211324865405187,  // (3.0 - sqrt(3.0)) / 6.0
                    0.366025403784439,  // 0.5 * (sqrt(3.0) - 1.0)
                   -0.577350269189626,  // -1.0 + 2.0 * C.x
                    0.024390243902439); // 1.0 / 41.0

vec3 taylorInvSqrt(vec3 r) { return 1.79284291400159 - 0.85373472095314 * r; }

float snoise(vec2 v) 
{
    // First corner
    vec2 i = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);

    // Other corners
    vec2 i1;
    i1.x = step(x0.y, x0.x);
    i1.y = 1.0 - i1.x;

    vec2 x1 = x0 - i1 + C.xx;
    vec2 x2 = x0 + C.zz;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));

    vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0.0);
    m = m * m;
    m = m * m;

    // Gradients
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    m *= taylorInvSqrt(a0 * a0 + h * h);

    // Compute final noise value at P
    vec3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.y = a0.y * x1.x + h.y * x1.y;
    g.z = a0.z * x2.x + h.z * x2.y;
    return (130.0 * dot(m, g)) * 0.5 + 0.5;
}

void main()
{
    uint id = gl_GlobalInvocationID.x;

    if (id >= u_heightMapSize)
        return;

    int x = int(id) % u_mapSize;
    int y = int(id) / u_mapSize;

    float scale = u_scaleFactor;
    float weight = 1.0;
    for (int i = 0; i < u_octaves; i++) 
    {
        heightMap.Data[id] += snoise(vec2(float(x), float(y))/u_mapSize * scale + offsets.Data[i]) * weight;
        scale *= u_lacunarity;
        weight *= u_persistence;
    }

    int val = int(heightMap.Data[id] * u_floatToIntMultiplier);
    atomicMin(minMax.Data[0], val);
    atomicMax(minMax.Data[1], val); 
}