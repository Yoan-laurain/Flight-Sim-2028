#pragma once

#include "Library/coreMinimal.h"
#include "Vendor/json/json.h"
#include <vector>

class Texture;
class Model;
struct Vertex;
enum class ShaderType;

using json = nlohmann::json;

#define DEFAULT_TEXTURE_PATH "res/textures/default.jpg"

class ModelLoader
{
    public:
        ModelLoader(const char* file, ShaderType shaderType);
        ~ModelLoader();

        std::unique_ptr<Model> m_Model;
        ShaderType m_ShaderType;

    private:

        // Loads a single mesh by its index
        void LoadMesh(unsigned int indMesh);

        // Traverses a node recursively, so it essentially traverses all connected nodes
        void TraverseNode(unsigned int nextNode, const Mat4<float>& matrix = Mat4(1.0f));

        // Gets the binary data from a file
        std::vector<unsigned char> GetData();
	    
        // Interprets the binary data into floats, indices, and textures
        std::vector<float> GetFloats(json accessor);
        std::vector<unsigned> GetIndices(json accessor);
        std::vector<Texture> GetTextures();

        // Assembles all the floats into vertices
        std::vector<Vertex> AssembleVertices(const std::vector<Vec3<float>>& positions, const std::vector<Vec3<float>>& normals, const std::vector<Vec2<float>>& texUVs );

        // Helps with the assembly from above by grouping floats
        std::vector<Vec2<float>> GroupFloatsVec2(const std::vector<float>& floatVec);
        std::vector<Vec3<float>> GroupFloatsVec3(const std::vector<float>& floatVec);
        std::vector<Vec4<float>> GroupFloatsVec4(const std::vector<float>& floatVec);

        // Variables for easy access
        const char* m_file;
        std::vector<unsigned char> m_data;
        json m_json;

        std::vector<Vec3<float>> m_translationsMeshes;
        std::vector<Quaternion<float>> m_rotationsMeshes;
        std::vector<Vec3<float>> m_scalesMeshes;
        std::vector<Mat4<float>> m_matricesMeshes;

        // Prevents textures from being loaded twice
        std::vector<std::string> m_loadedTexName;
        std::vector<Texture> m_loadedTex;
};