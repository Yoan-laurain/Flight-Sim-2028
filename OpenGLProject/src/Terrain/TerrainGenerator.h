#pragma once

#include <vector>
#include <memory>

class ShaderStorageBufferObject;
class TerrainModel;

class TerrainGenerator
{
    public:
        TerrainGenerator();
        ~TerrainGenerator();
            
        void Init(int width, int height);
        void Run();

        std::vector<float> m_HeightMap;

        float m_Scale = 2;
            
        // --------------- HeightMap ---------------

        int m_NumOctaves = 7;
        float m_Persistence = .5f;
        float m_Lacunarity = 4;
        float m_InitialScale = 20;
        float m_ElevationScale = 1;

        int m_Width;
        int m_Height;

    private:
    
        void GenerateHeightMap();
        
        std::unique_ptr<ShaderStorageBufferObject> m_OffsetsBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_MapBuffer;
        std::unique_ptr<ShaderStorageBufferObject> m_MinMaxBuffer;
};