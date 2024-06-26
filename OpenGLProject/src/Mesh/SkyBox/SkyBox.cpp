#include "SkyBox.h"
#include "Core/Application.h"
#include "OpenGL/Textures/CubeMap/CubeMapTexture.h"
#include "OpenGL/VertexBuffer/VertexBuffer.h"
#include "Config.h"
#include <string>

SkyBox::SkyBox()
{
    const std::vector vertices =
    {
        Vertex(Vec3(-1.0f, -1.0f,  1.0f), Vec3(0.0f, 0.0f, 0.0f),Vec3(0.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f)),
        Vertex(Vec3(1.0f, -1.0f,  1.0f), Vec3(0.0f, 0.0f, 0.0f),Vec3(0.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f)),
        Vertex(Vec3(1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f),Vec3(0.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f)),
        Vertex(Vec3(-1.0f, -1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f),Vec3(0.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f)),
        Vertex(Vec3(-1.0f,  1.0f,  1.0f), Vec3(0.0f, 0.0f, 0.0f),Vec3(0.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f)),
        Vertex(Vec3(1.0f,  1.0f,  1.0f), Vec3(0.0f, 0.0f, 0.0f),Vec3(0.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f)),
        Vertex(Vec3(1.0f,  1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f),Vec3(0.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f)),
        Vertex(Vec3(-1.0f,  1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f),Vec3(0.0f, 0.0f, 0.0f),Vec2(0.0f, 0.0f))
    };

    const std::vector<unsigned int> indices =
    {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3
    };

    // All the faces of the cubemap (make sure they are in this exact order)
    std::string facesCubemap[6] =
    {
        "res/skybox/right.jpg",
        "res/skybox/left.jpg",
        "res/skybox/top.jpg",
        "res/skybox/bottom.jpg",
        "res/skybox/front.jpg",
        "res/skybox/back.jpg"
    };

    const CubeMapTexture textureCubemap(facesCubemap, 6, Diffuse);

    m_Textures.emplace_back(textureCubemap); 

    m_Vertices = vertices;
    m_Indices = indices;
    Application::Get()->m_TriangleCount += static_cast<int>(indices.size() / 3);

    m_Matrix = Mat4(1.0f);
}

SkyBox::~SkyBox() = default;