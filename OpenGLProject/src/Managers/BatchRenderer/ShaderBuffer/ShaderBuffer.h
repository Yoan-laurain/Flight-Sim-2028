﻿#pragma once

#include <map>

#include "../../../Library/coreMinimal.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class Mesh;
class Model;
class Shader;
class Texture;
class VertexBuffer;
class IndexBuffer;
class VertexArray;
class ShaderStorageBufferObject;
struct Vertex;

enum class ShaderType;

struct TransformData 
{
    Mat4<float> transform;
    Mat4<float> rotation;
    Mat4<float> scale;
    Mat4<float> model;
};
/*
 * Todo : Adapt code to allow Texture blending & multiple (diffuse or specular) textures per mesh 
 */
struct ShadersBuffer
{
    public:
 
        ShadersBuffer(Mesh* mesh, Model* model, int indexOfBuffer);

        /*
         * Create the vertex buffer, index buffer , shader storage buffer object and fill the arrays
         * of Texture slot to use for this buffer also fill the transform data array
         */
        void Init();

        /*
         * Add a new mesh to the buffer
         */
        void AddNewMesh(Mesh* mesh, Model* model);

        /*
        * Fill the transform data array for the buffer to be used by SSBO to send data to the shader for the specified model
        */
        void ExtractModelTransformData(const std::vector<Model*>::value_type& model);

        /*
         * Update the vertices of the model in the buffer
         */
        void UpdateModelVerticesDatas(Model* model);
 
        /*
         * Set the textures uniforms and bind the textures to the shader
         * Need to be called for each draw since we can have multiple draw with the same shader
         */
        void SetTexturesUniforms(ShaderType,Shader* shader) const;

        /* Concatenation of all the vertices of all the meshes of this buffer */
        std::vector<Vertex> m_Vertices;

        /* Concatenation of all the indices of all the meshes of this buffer */
        std::vector<unsigned> m_Indices;

        /* List of all the textures that will be used for this buffer */
	       std::unordered_map<std::string,Texture> m_TexturesDiffuse = {};
        std::unordered_map<std::string,Texture> m_TexturesSpecular = {}; 

        /* List of all the models that will be drawn for this buffer */
        std::vector<Model*> m_Models;
        
        std::unique_ptr<VertexBuffer> m_VBO;
        std::unique_ptr<IndexBuffer> m_IBO;
        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<ShaderStorageBufferObject> m_SSBO;

        /* List of transform data for each mesh of the buffer that will be send to the shader by the SSBO */
        std::map<int, std::vector<TransformData>> m_ModelsTransforms;

        std::unordered_map<Model*, std::vector<Vertex>> m_ModelsVertices;

        std::vector<TransformData>& GetAllTransformsData();
 
        /* Index of the buffer for the current shader type */
        int m_IndexBuffer = 0;

    private:

       std::vector<TransformData> m_transformsData;
       bool m_needToRegroupTransformsAgain = false;

       /*
       * Fill the arrays of diffuse and specular slot to use for this buffer
       */
       void InitTextureUniform();

       /*
       * Add the textures of the mesh to the list of textures of the buffer
       */
       void AddTextureToTheMap(std::unordered_map<std::string,Texture>& textures, const std::vector<std::unique_ptr<Texture>>& texture);

        /*
         * Fill the array of Texture slot to use for this buffer for the specified array of textures
         */
        void CreateTextureUniforms(const std::unordered_map<std::string, Texture>& texturesMap, std::vector<int>& array);

       /*
       * Set the index of Specular and diffuse Texture to use for the current mesh
       */
       void SetIndexOfTextures(Mesh* mesh);

       /*
       * Get the index of the Texture if it already exist, otherwise return the next available index
       */
       int GetIndexForTexture(const Mesh* mesh, bool isSpecular);

        /*
        * Adapt the indices buffer of the current mesh to match offset apply by the existence of other meshes
        */
        void InsertIndices(const Mesh* mesh);

        /*
        * Set the index of the model associated to this mesh to allow shader to determine which values to use in the arrays
        */
        void SetModelIndexForVertexBuffers(Mesh* mesh) const;

        /*
         * Set the textures uniforms and bind the textures to the shader for the specified array of textures
         */
        void SetUniformAndBind(Shader* shader, const std::string& uniformName,
                               const std::vector<int>& arraySlot,
                               const std::unordered_map<std::string, Texture>& textures) const;

        /* List of slot used by the shader to bind the diffuses textures */
        std::vector<int> m_diffuse;

        /* List of slot used by the shader to bind the specular textures */
        std::vector<int> m_specular;
};