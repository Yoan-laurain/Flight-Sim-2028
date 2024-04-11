#pragma once

#include "../Library/coreMinimal.h"
#include "Vendor/json/json.h"

#include <GL/glew.h>
#include <vector>

class Texture;
class Model;
struct Vertex;
enum class ShaderType;

using json = nlohmann::json;

class ModelLoader
{
    public:
        explicit ModelLoader(const char* file, ShaderType shaderType);
        ~ModelLoader();

        std::unique_ptr<Model> m_Model;
        ShaderType m_ShaderType;

    private:

        // Loads a single mesh by its index
        void LoadMesh(unsigned int indMesh);

        // Traverses a node recursively, so it essentially traverses all connected nodes
        void TraverseNode(unsigned int nextNode, const Mat4<float>& matrix = Mat4<float>(1.0f));

        // Gets the binary data from a file
        std::vector<unsigned char> GetData();
	    
        // Interprets the binary data into floats, indices, and textures
        std::vector<float> GetFloats(json accessor);
        std::vector<GLuint> GetIndices(json accessor);
        std::vector<Texture> GetTextures();

        // Assembles all the floats into vertices
        std::vector<Vertex> AssembleVertices(const std::vector<Vec3<float>>& positions, const std::vector<Vec3<float>>& normals, const std::vector<Vec2<float>>& texUVs );

        // Helps with the assembly from above by grouping floats
        std::vector<Vec2<float>> GroupFloatsVec2(const std::vector<float>& floatVec);
        std::vector<Vec3<float>> GroupFloatsVec3(const std::vector<float>& floatVec);
        std::vector<Vec4<float>> GroupFloatsVec4(const std::vector<float>& floatVec);

        // Variables for easy access
        const char* m_File;
        std::vector<unsigned char> m_Data;
        json m_JSON;

        std::vector<Vec3<float>> m_TranslationsMeshes;
        std::vector<Quaternion<float>> m_RotationsMeshes;
        std::vector<Vec3<float>> m_ScalesMeshes;
        std::vector<Mat4<float>> m_MatricesMeshes;

        // Prevents textures from being loaded twice
        std::vector<std::string> m_LoadedTexName;
        std::vector<Texture> m_LoadedTex;
};
