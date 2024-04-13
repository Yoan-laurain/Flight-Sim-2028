#include "ShaderData.h"
#include "Core/Application.h"
#include "Library/Math.h"
#include "Model/Model.h"
#include "OpenGL/IndexBuffer/IndexBuffer.h"
#include "OpenGL/Mesh/Mesh.h"
#include "OpenGL/Shader/Shader.h"
#include "OpenGL/ShaderStorageBuffer/ShaderStorageBufferObject.h"
#include "OpenGL/Textures/Texture.h"
#include "OpenGL/VertexArray/VertexArray.h"
#include "OpenGL/VertexBuffer/VertexBuffer.h"

ShadersData::ShadersData(Mesh* mesh, Model* model, const int index, int textureIndex)
    : m_Vertices({})
    , m_Indices({})
    , m_VBO(nullptr)
    , m_IBO(nullptr)
    , m_IndexBuffer(index)
{
    m_VAO = std::make_unique<VertexArray>();
    
    AddNewMesh(mesh, model,textureIndex);
}

void ShadersData::AddNewMesh(Mesh* mesh, Model* model, int textureIndex)
{
    SetIndexOfTextures(mesh,textureIndex);
    
    InsertIndices(mesh);
    m_Vertices.insert(m_Vertices.end(), mesh->m_Vertices.begin(), mesh->m_Vertices.end());
    
    // if the model is not already in the list, add it ( Since a model can have multiple meshes )
    if (std::find_if(m_Models.begin(), m_Models.end(), [&model](const auto& m) { return m.get() == model; }) == m_Models.end())
    {
        m_Models.push_back(std::shared_ptr<Model>(model));
    }

    AddTexture(m_TexturesDiffuse, mesh->m_TextureDiffuse.get());
    AddTexture(m_TexturesSpecular, mesh->m_TextureSpecular.get());
}

void ShadersData::Init()
{
    m_VBO = std::make_unique<VertexBuffer>(m_Vertices);
    m_IBO = std::make_unique<IndexBuffer>(m_Indices);
    m_SSBO = std::make_unique<ShaderStorageBufferObject>();
    
    ExtractTransformData();
    
    InitTextureUniform();
}

void ShadersData::AddTexture(std::unordered_map<std::string, Texture>& textures, Texture* texture)
{
    if (texture && !textures.contains(texture->m_FilePath))
    {
        texture->m_Index = textures.size();
        textures[texture->m_FilePath] = *texture;
    }
}

void ShadersData::ExtractTransformData()
{
    for (const auto& model : m_Models)
    {
        for (const auto& mesh : model->m_Meshes)
        {
            TransformData transformData 
            {
                Math::translate(Mat4(1.0f), model->GetTranslation()),
                Math::mat4_cast(Quaternion(model->GetRotation())),
                Math::Scale(Mat4(1.0f), model->GetScale()),
                mesh->m_Matrix
            };

            transformsDatas.push_back(transformData);
        }
    }
}

void ShadersData::InitTextureUniform()
{
    const int MaxSlotForTextures = Application::Get()->GetMaxSlotForTextures();
    
    CreateTextureUniforms(m_TexturesDiffuse, m_Diffuse, MaxSlotForTextures);
    CreateTextureUniforms(m_TexturesSpecular, m_Specular, MaxSlotForTextures);
}

void ShadersData::CreateTextureUniforms(const std::unordered_map<std::string, Texture>& texturesMap, std::vector<int>& textureUniforms, const int maxSlotForTextures)
{
    for (const auto& texture : texturesMap)
    {
        textureUniforms.push_back(texture.second.m_Slot - maxSlotForTextures * m_IndexBuffer);
    }
}

void ShadersData::SetIndexOfTextures(Mesh* mesh, int index)
{
    int indexSpecular = index;
    int indexDiffuse = index;
    if ( index == -1 )
    {
        indexSpecular = mesh->m_TextureSpecular ? GetIndexOfTextureIfAlreadyExisting(mesh, true) : 0;
        indexDiffuse = mesh->m_TextureDiffuse ? GetIndexOfTextureIfAlreadyExisting(mesh, false) : 0;
    }
    
    // Set the index of the texture in the vertices
    for (auto& vertex : mesh->m_Vertices) 
    {
        vertex.m_IndexSpecular = indexSpecular;
        vertex.m_IndexDiffuse = indexDiffuse;
    }
}

int ShadersData::GetIndexOfTextureIfAlreadyExisting(const Mesh* mesh, bool isSpecular)
{
    int index = 0;
    
    auto& texturesMap = isSpecular ? m_TexturesSpecular : m_TexturesDiffuse; // Retrieve the map of textures corresponding to the type
    const auto& texture = isSpecular ? mesh->m_TextureSpecular : mesh->m_TextureDiffuse; // Retrieve the texture of the mesh corresponding to the type

    if (texture && texturesMap.contains(texture->m_FilePath)) // Check if the texture is already in the map
    {
        index = texturesMap[texture->m_FilePath].m_Index; // Retrieve the index of the texture
    }
    else
    {
        index = isSpecular ? m_TexturesSpecular.size() : m_TexturesDiffuse.size(); // Retrieve the next available index for the texture
    }

    return index;
}

void ShadersData::InsertIndices(const Mesh* mesh) 
{
    const int size = m_Vertices.size();
    
    m_Indices.reserve(m_Indices.size() + mesh->m_Indices.size());
    
    for (const auto& index : mesh->m_Indices)
    {
        m_Indices.push_back(index + size);
    }
}

void ShadersData::BindTexturesToShader(ShaderType, Shader* shader)
{
    shader->Bind();

    if (!m_TexturesDiffuse.empty()) 
    {
        shader->SetUniform1iv("u_Diffuse", m_Diffuse); 
    }

    // Set the uniform for the specular
    if (!m_TexturesSpecular.empty())
    {
        shader->SetUniform1iv("u_Specular", m_Specular); 
    }
    
    int MaxSlotForTextures = Application::Get()->GetMaxSlotForTextures();
    for (auto& texture : m_TexturesDiffuse)
    {
        texture.second.Bind( texture.second.m_Slot - MaxSlotForTextures * m_IndexBuffer );  
    } 

    for (auto& texture : m_TexturesSpecular)
    {
        texture.second.Bind( texture.second.m_Slot - MaxSlotForTextures * m_IndexBuffer ); 
    } 
}