#pragma once

#include "../Library/coreMinimal.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class VertexBuffer;
class IndexBuffer;
struct Vertex;
class VertexArray;
class VertexBufferLayout;
class Shader;
class Model;
class Mesh;
class ShaderStorageBufferObject;
class Texture;

enum class ShaderType;

struct TransformData 
{
    Mat4<float> transform;
    Mat4<float> rotation;
    Mat4<float> scale;
    Mat4<float> model;
};

struct ShadersData
{
    ShadersData(Mesh* mesh, Model* model);

    void Init(ShaderType, Shader* shader);
    void AddNewMesh(Mesh* mesh, Model* model);
    
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned> m_Indices;

	std::unordered_map<std::string,Texture> m_TexturesDiffuse = {};
    std::unordered_map<std::string,Texture> m_TexturesSpecular = {}; 

    std::vector<std::shared_ptr<Model>> m_Models;
    
    int m_currentIndexTextureDiffuse; 		
    int m_currentIndexTextureSpecular;	
    
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer> m_IBO;
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<ShaderStorageBufferObject> m_SSBO;

    std::vector<TransformData> transformsDatas;
    
    private:

        std::vector<int> m_Diffuse;
        std::vector<int> m_Specular;
    
        void ExtractModelData();
        void InitTextureUniform(ShaderType,Shader* shader);
};

class BatchRenderer
{
    public:
        BatchRenderer();
        ~BatchRenderer();
    
        void AddNewMesh(Model* model, Mesh* mesh);
    
        int Draw();
    
        /* TODO Handle if texture slot > MaxSlotForTextures*/
        int GetNextIndexToBindTextureTo();

        Texture* CreateOrGetTexture( const char* image, const char* texType, ShaderType shaderType);

    private:
        int indexToBindTextureTo;
        int MaxSlotForTextures;
       
        std::unique_ptr<VertexBufferLayout> m_Layout;
        std::unordered_map<ShaderType,std::unique_ptr<ShadersData>> m_ShadersData;
    
        void AdaptIndices(ShaderType shaderType, Mesh* mesh);
        void SetModelIndexForVertexBuffers(ShaderType shaderType, Mesh* mesh);
        void SetIndexOfTextures(ShaderType shaderType, Mesh* mesh);
        int GetIndexOfTextureIfAlreadyExisting(ShaderType shaderType, const Mesh* mesh, bool isSpecular);
};