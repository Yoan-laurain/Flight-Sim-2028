#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

struct Vertex;
class VertexBufferLayout;
class Shader;
class Model;
class Mesh;
class Texture;
class ShadersBuffer;

enum class ShaderType;

class BatchRenderer
{
    public:
        BatchRenderer();
        ~BatchRenderer();

        /*
         * Need to be called once per frame, this will draw all the meshes registered
         */
        int Draw();

        /*
         * Add a new mesh to the batch renderer, so it will be drawn in the game
         */
        void AddNewMesh(Model* model, Mesh* mesh);

        /*
         * Retrieve the next available index that a texture can be bind for a shader 
         */
        int GetNextIndexToBindTextureTo(ShaderType shaderType);

        /*
         * Create and return a new Texture, if the texture already exist, it will return the existing one to avoid duplication
         */
        Texture* CreateOrGetTexture( const char* image, const char* texType, ShaderType shaderType);

        /*
         * Method to call when an object is moved,scaled or rotated to update the model matrix
         */
        void UpdateModelDatas(Model* model, ShaderType shaderType);

    private:

        /* Keep track of the current available texture slot for each shader */
        std::unordered_map<ShaderType,int> m_IndexTextures;

        /* Layout of the vertex buffer */
        std::unique_ptr<VertexBufferLayout> m_Layout;

        /* Store all the required data to draw shapes for each shader */
        std::unordered_map<ShaderType,std::vector<std::unique_ptr<ShadersBuffer>>> m_ShadersData;

        /*
         * Create a new buffer for the shader type if needed, if the buffer does not exist at all or if we have reached the maximum slot of textures
         */
        bool CreateNewBufferIfNeed(ShaderType shaderType, Mesh* mesh,Model* model);

        /*
         * Methods that will be called by AddNewMesh to add the mesh to the appropriate buffer
         * Allow to regroup same textures in the same buffer to reduce the number of draw calls
         */
        void AddToAppropriateBuffer(Model* model, Mesh* mesh);
        bool AddToBufferIfAlreadyExisting( Model* model, Mesh* mesh);
        void AddToFirstBufferWhoCanHoldIt( Model* model, Mesh* mesh);
};