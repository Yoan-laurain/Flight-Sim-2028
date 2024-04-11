#include "BatchRenderer.h"
#include "../OpenGL/VertexBuffer/VertexBuffer.h"
#include "../OpenGL/IndexBuffer/IndexBuffer.h"
#include "../OpenGL/VertexArray/VertexArray.h"
#include "../OpenGL/VertexBufferLayout/VertexBufferLayout.h"
#include "../OpenGL/ShaderStorageBuffer/ShaderStorageBufferObject.h"
#include "../OpenGL/Shader/Shader.h"
#include "../OpenGL/Mesh/Mesh.h"
#include "../OpenGL/Textures/Texture.h" 
#include "ShaderManager.h"
#include "../Core/Application.h"
#include "../Renderer/Renderer.h"
#include "../Model/Model.h"
#include "../Config.h"
#include "Library/Math.h"

#include <vector>
#include <chrono>
#include <iostream>

BatchRenderer::BatchRenderer() : indexToBindTextureTo(0), MaxSlotForTextures(0)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Layout->Push<float>(3); // m_Position
    m_Layout->Push<float>(3); // Normal
    m_Layout->Push<float>(3); // Colors
    m_Layout->Push<float>(2); // Texture coordinates;
    m_Layout->Push<float>(1); // index;
    m_Layout->Push<float>(1); // index diffuse
    m_Layout->Push<float>(1); // index specular

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxSlotForTextures);
}

BatchRenderer::~BatchRenderer() = default;

void BatchRenderer::AddNewMesh(Model* model, Mesh* mesh)
{
    const ShaderType shaderType = model->m_ShaderType;

    // Set indices for vertex buffers and adapt indices
    SetModelIndexForVertexBuffers(shaderType, mesh);
    AdaptIndices(shaderType, mesh);
    SetIndexOfTextures(shaderType, mesh);

    // Check if shader data already exists
    auto shaderDataIter = m_ShadersData.find(shaderType);
    if (shaderDataIter == m_ShadersData.end())
    {
        // If not, create new shader data
        m_ShadersData[shaderType] = std::make_unique<ShadersData>(ShadersData(mesh, model));  
    }
    else
    {
        // If yes, add new mesh to existing shader data
        shaderDataIter->second->AddNewMesh(mesh, model);
    }
}

int BatchRenderer::Draw()
{
    int drawCalls = 0;
    
    for (auto& shaderData : m_ShadersData)
    {
		Shader* shader = Application::Get()->GetShaderManager()->GetShader(shaderData.first); 
        
        if (m_ShadersData[shaderData.first]->m_VBO == nullptr) // If the VBO is not initialized, initialize it
        {
			m_ShadersData[shaderData.first]->Init( shaderData.first, shader );
		}
       
        m_ShadersData[shaderData.first]->m_VAO->Bind(); 
        m_ShadersData[shaderData.first]->m_VAO->AddBuffer(*m_ShadersData[shaderData.first]->m_VBO, *m_Layout);
        
        shader->OnBeforeDraw();
        Application::Get()->GetRenderer()->Draw( *m_ShadersData[shaderData.first]->m_VAO, *m_ShadersData[shaderData.first]->m_IBO, *shader);
        shader->OnAfterDraw();
        
        drawCalls++;
	}
    
    return drawCalls;
}

void BatchRenderer::AdaptIndices(const ShaderType shaderType, Mesh* mesh)
{
    // If it is the first mesh of the shader type, the offset is already 0 so no need to change it
    auto shaderDataIter = m_ShadersData.find(shaderType);
    if (shaderDataIter == m_ShadersData.end())
        return;

    // Get the offset that need to be added to the indices to find the correct vertex
    const int vertexOffset = shaderDataIter->second->m_Vertices.size();

    for (auto& index : mesh->m_Indices)
    {
        index += vertexOffset;
    }
}

Texture* BatchRenderer::CreateOrGetTexture(const char* image, const char* texType, ShaderType shaderType)
{
     if (m_ShadersData.find(shaderType) != m_ShadersData.end())
     {
         if (strcmp(texType, Diffuse) == 0)
         {
             if (m_ShadersData[shaderType]->m_TexturesDiffuse.find(image) != m_ShadersData[shaderType]->m_TexturesDiffuse.end())
             {
                 return new Texture( m_ShadersData[shaderType]->m_TexturesDiffuse[image] ) ;
             }
         }
         else if (strcmp(texType, Specular) == 0)
         {
             if (m_ShadersData[shaderType]->m_TexturesSpecular.find(image) != m_ShadersData[shaderType]->m_TexturesSpecular.end())
             {
                 return new Texture(  m_ShadersData[shaderType]->m_TexturesSpecular[image] ) ;
             }
         }
     }
    
    return new Texture(image, texType);
}

int BatchRenderer::GetNextIndexToBindTextureTo()
{
    int index = indexToBindTextureTo;
    indexToBindTextureTo++;
    return index;
}

void BatchRenderer::SetModelIndexForVertexBuffers(const ShaderType shaderType, Mesh* mesh)
{
    // If it is the first mesh of the shader type, the index is already 0 so no need to change it
    const auto shaderDataIter = m_ShadersData.find(shaderType);
    if (shaderDataIter == m_ShadersData.end())
        return;

    // Get the 'id' of the current model
    const int index = shaderDataIter->second->m_Models.size();

    // Set the index of the model in the vertices
    for (auto& vertex : mesh->m_Vertices)
    {
        vertex.m_IndexModel = index;
    }
}

void BatchRenderer::SetIndexOfTextures(const ShaderType shaderType, Mesh* mesh)
{
    const int indexSpecular = mesh->m_TextureSpecular ? GetIndexOfTextureIfAlreadyExisting(shaderType, mesh, true) : 0;
    const int indexDiffuse = mesh->m_TextureDiffuse ? GetIndexOfTextureIfAlreadyExisting(shaderType, mesh, false) : 0;
    
    // Set the index of the texture in the vertices
    for (auto& vertex : mesh->m_Vertices) 
    {
        vertex.m_IndexSpecular = indexSpecular;
        vertex.m_IndexDiffuse = indexDiffuse;
    }
}

int BatchRenderer::GetIndexOfTextureIfAlreadyExisting(const ShaderType shaderType, const Mesh* mesh, const bool isSpecular)
{
    int index = 0;

    if (m_ShadersData.find(shaderType) != m_ShadersData.end()) // Check if shader data exists
    {
        auto& shaderData = m_ShadersData[shaderType]; 
        auto& texturesMap = isSpecular ? shaderData->m_TexturesSpecular : shaderData->m_TexturesDiffuse; // Retrieve the map of textures corresponding to the type
        const auto& texture = isSpecular ? mesh->m_TextureSpecular : mesh->m_TextureDiffuse; // Retrieve the texture of the mesh corresponding to the type

        if (texture && texturesMap.find(texture->m_FilePath) != texturesMap.end()) // Check if the texture is already in the map
        {
            index = texturesMap[texture->m_FilePath].m_Index; // Retrieve the index of the texture
        }
        else
        {
            index = isSpecular ? shaderData->m_currentIndexTextureSpecular : shaderData->m_currentIndexTextureDiffuse; // Retrieve the next available index for the texture
        }
    }

    return index;
}

/*
 * ShadersData
 */

ShadersData::ShadersData(Mesh* mesh, Model* model)
    : m_Vertices({})
    , m_Indices({})
    , m_currentIndexTextureDiffuse(0)
    , m_currentIndexTextureSpecular(0)
    , m_VBO(nullptr)
    , m_IBO(nullptr)
{
    m_VAO = std::make_unique<VertexArray>();

    AddNewMesh(mesh, model);
}

void ShadersData::AddNewMesh(Mesh* mesh, Model* model)
{
    // Add the vertices and indices to the list
    m_Vertices.insert( m_Vertices.end(), mesh->m_Vertices.begin(), mesh->m_Vertices.end() );
    m_Indices.insert( m_Indices.end(), mesh->m_Indices.begin(), mesh->m_Indices.end() );

    std::shared_ptr<Model> modelPtr(model);

    // if the model is not already in the list, add it ( Since a model can have multiple meshes )
    if (std::find(m_Models.begin(), m_Models.end(), modelPtr) == m_Models.end())
    {
        m_Models.push_back( modelPtr );
    }

    // Add the diffuse texture to the list of textures
    if (mesh->m_TextureDiffuse && m_TexturesDiffuse.find(mesh->m_TextureDiffuse->m_FilePath) == m_TexturesDiffuse.end())
    {
        mesh->m_TextureDiffuse->m_Index = m_currentIndexTextureDiffuse;
        m_TexturesDiffuse[mesh->m_TextureDiffuse->m_FilePath] = *mesh->m_TextureDiffuse;
        m_currentIndexTextureDiffuse++;
    }
    
    // Add the specular texture to the list of textures
    if (mesh->m_TextureSpecular && m_TexturesSpecular.find(mesh->m_TextureSpecular->m_FilePath) == m_TexturesSpecular.end())
    {
        mesh->m_TextureSpecular->m_Index = m_currentIndexTextureSpecular;
        m_TexturesSpecular[mesh->m_TextureSpecular->m_FilePath] = *mesh->m_TextureSpecular;
        m_currentIndexTextureSpecular++;
    }
}

void ShadersData::Init(ShaderType shaderType, Shader* shader)
{
    m_VBO = std::make_unique<VertexBuffer>(m_Vertices);
    m_IBO = std::make_unique<IndexBuffer>(m_Indices);
    
    ExtractModelData();

    m_SSBO = std::make_unique<ShaderStorageBufferObject>();
    m_SSBO->SetData( transformsDatas.data(), sizeof(TransformData) * transformsDatas.size(), 7);

    InitTextureUniform(shaderType,shader);
}

void ShadersData::ExtractModelData()
{
    for (const auto& model : m_Models)
    {
        for (const auto& mesh : model->m_Meshes)
        {
            TransformData transformData 
            {
                Math::translate(Mat4(1.0f), model->GetTranslation()),
                Math::mat4_cast(Quaternion(model->GetRotation())),
                Math::Scale(Mat4<float>(1.0f), model->GetScale()),
                mesh->m_Matrix
            };

            transformsDatas.push_back(transformData);
        }
    }
}

void ShadersData::InitTextureUniform(ShaderType, Shader* shader)
{
    // Create the uniform for the diffuse
    for (auto& texture : m_TexturesDiffuse) 
    {
        m_Diffuse.push_back(texture.second.m_Slot);
        texture.second.Bind();
    }

    // Create the uniform for the specular
    for (auto& texture : m_TexturesSpecular)
    {
        m_Specular.push_back(texture.second.m_Slot);
        texture.second.Bind();
    }

    shader->Bind();

    // Set the uniform for the diffuse
    if (!m_TexturesDiffuse.empty())
    {
        shader->SetUniform1iv("u_Diffuse", m_Diffuse); 
    }

    // Set the uniform for the specular
    if (!m_TexturesSpecular.empty())
    {
        shader->SetUniform1iv("u_Specular", m_Specular); 
    }
}