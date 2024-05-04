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
struct ShadersBuffer;

enum class ShaderType;

enum class UpdateType
{
    Transforms,
    Vertices
};

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
         * Retrieve the next available index that a Texture can be bind for a shader 
         */
        int GetNextIndexToBindTextureTo(ShaderType shaderType);

        /*
         * Create and return a new Texture, if the Texture already exist, it will return the existing one to avoid duplication
         */
        Texture* CreateOrGetTexture(const char* image, const char* texType, ShaderType shaderType);
 
        /*
         * Method to call when an object is moved,scaled or rotated to update the model matrix
         * or when the vertices of a model are updated
         */
         void UpdateModelData(Model* model, ShaderType shaderType, UpdateType updateType);

    private:

        /* Keep track of the current available Texture slot for each shader */
        std::unordered_map<ShaderType,int> m_indexTextures;

        /* Layout of the vertex buffer */
        std::unique_ptr<VertexBufferLayout> m_layout;

        /* Store all the required data to draw shapes for each shader */
        std::unordered_map<ShaderType,std::vector<std::unique_ptr<ShadersBuffer>>> m_shadersData;

        /*
         * Create a new buffer for the shader type if needed, if the buffer does not exist at all or if we have reached the maximum slot of textures
         */
        bool CreateNewBufferIfNeed(ShaderType shaderType, Mesh* mesh,Model* model);

        /*
         * Methods that will be called by AddNewMesh to add the mesh to the appropriate buffer
         * Allow to regroup same textures in the same buffer to reduce the number of draw calls
         */
        void AddToAppropriateBuffer(Model* model, Mesh* mesh);
        bool AddToBufferIfAlreadyExisting(Model* model, Mesh* mesh);
        bool IsMeshAlreadyInBuffer(const std::unique_ptr<ShadersBuffer>& array, const Mesh* mesh) const;
        void AddToFirstBufferWhoCanHoldIt(Model* model, Mesh* mesh);
};