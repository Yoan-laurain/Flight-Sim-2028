#include "ShaderBuffer.h"
#include "Core/Application.h"
#include "Library/Math.h"
#include "Model/Model.h"
#include "OpenGL/IndexBuffer/IndexBuffer.h"
#include "../../../Mesh/Mesh.h"
#include "OpenGL/Shader/Shader.h"
#include "OpenGL/ShaderStorageBuffer/ShaderStorageBufferObject.h"
#include "OpenGL/Textures/Texture.h"
#include "OpenGL/VertexArray/VertexArray.h"
#include "OpenGL/VertexBuffer/VertexBuffer.h"

ShadersBuffer::ShadersBuffer(Mesh* mesh, Model* model, const int indexOfBuffer)
    : m_Vertices({})
    , m_Indices({})
    , m_VBO(nullptr)
    , m_IBO(nullptr)
    , m_IndexBuffer(indexOfBuffer)
{
    m_VAO = std::make_unique<VertexArray>();
    
    AddNewMesh(mesh, model);
}

void ShadersBuffer::SetIndexOfTextures(Mesh* mesh)
{
     const int indexSpecular = GetSlotForTexture(mesh, true);
     const int indexDiffuse = GetSlotForTexture(mesh, false) ;
    
    for (auto& vertex : mesh->m_Vertices) 
    {
        vertex.m_IndexSpecular = indexSpecular;
        vertex.m_IndexDiffuse = indexDiffuse;
    }
}

void ShadersBuffer::AddNewMesh(Mesh* mesh, Model* model)
{
    SetModelIndexForVertexBuffers(mesh);
    SetIndexOfTextures(mesh);
    InsertIndices(mesh);
    
    m_Vertices.insert(m_Vertices.end(), mesh->m_Vertices.begin(), mesh->m_Vertices.end());
    m_ModelsVertices[model].insert(m_ModelsVertices[model].end(), mesh->m_Vertices.begin(), mesh->m_Vertices.end());
    
    // if the model is not already in the list, add it ( Since a model can have multiple meshes )
    if (std::ranges::find_if(m_Models, [model](const Model* m) { return m == model; }) == m_Models.end())
    {
        m_Models.emplace_back(model);
    }

    AddTexture(m_TexturesDiffuse, mesh->m_TextureDiffuse);
    AddTexture(m_TexturesSpecular, mesh->m_TextureSpecular);
}

void ShadersBuffer::Init()
{
    m_VBO = std::make_unique<VertexBuffer>(m_Vertices);
    m_IBO = std::make_unique<IndexBuffer>(m_Indices);
    m_SSBO = std::make_unique<ShaderStorageBufferObject>();
    
    for (const auto& model : m_Models)
    {
        ExtractModelTransformData(model);
    }
    
    InitTextureUniform();
} 

std::vector<TransformData>& ShadersBuffer::GetAllTransformsData()
{
    if ( m_NeedToRegroupTransformsAgain )
    {
        m_TransformsData.clear();
        for (const auto& [model, transforms] : m_ModelsTransforms)
        {
            m_TransformsData.insert(m_TransformsData.end(), transforms.begin(), transforms.end());
        }
        m_NeedToRegroupTransformsAgain = false;
    }
    return m_TransformsData;
}

void ShadersBuffer::AddTexture(std::unordered_map<std::string, Texture>& textures, const std::vector<std::unique_ptr<Texture>>& texture)
{
    const int size = static_cast<int>(textures.size());
    for (const auto& tex : texture)
    {
        if (tex && !textures.contains(tex->m_FilePath))
        {
            tex->m_Index = size;
            textures[tex->m_FilePath] = *tex;
        }
    }
}

void ShadersBuffer::ExtractModelTransformData(const std::vector<Model*>::value_type& model)
{
    if (m_ModelsTransforms.contains(model))
    {
        m_ModelsTransforms[model].clear();
    }
    
    for (const auto& mesh : model->m_Meshes)
    {
        TransformData transformData 
        {
            Math::translate(Mat4(1.0f), model->GetTranslation()),
            Math::mat4_cast(Quaternion(model->GetRotation())),
            Math::Scale(Mat4(1.0f), model->GetScale()),
            mesh->m_Matrix
        };

        if (m_ModelsTransforms.contains(model))
        {
            m_ModelsTransforms[model].emplace_back(transformData); 
        }
        else
        {
            m_ModelsTransforms[model] = { transformData }; 
        }
    }

    m_NeedToRegroupTransformsAgain = true;
}

void ShadersBuffer::UpdateModelVerticesDatas(Model* model)
{
    if (m_ModelsVertices.contains(model))
    {
        m_ModelsVertices[model].clear();
    }
    
    for (const auto& mesh : model->m_Meshes)
    {
        if (m_ModelsVertices.contains(model))
        {
            m_ModelsVertices[model].insert(m_ModelsVertices[model].end(), mesh->m_Vertices.begin(), mesh->m_Vertices.end());
        }
        else
        {
            m_ModelsVertices[model] = mesh->m_Vertices;
        }
    }
    
    m_Vertices.clear();

    for (const auto& [model, vertices] : m_ModelsVertices)
    {
        m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
    }

    m_VBO->SetDatas(m_Vertices);
}

void ShadersBuffer::InitTextureUniform()
{
    const int MaxSlotForTextures = Application::Get()->GetMaxSlotForTextures();
    
    CreateTextureUniforms(m_TexturesDiffuse, m_Diffuse, MaxSlotForTextures);
    CreateTextureUniforms(m_TexturesSpecular, m_Specular, MaxSlotForTextures);
}

void ShadersBuffer::CreateTextureUniforms(const std::unordered_map<std::string, Texture>& texturesMap, std::vector<int>& textureUniforms, const int maxSlotForTextures)
{
    for (const auto& texture : texturesMap)
    {
        textureUniforms.emplace_back(texture.second.m_Slot - maxSlotForTextures * m_IndexBuffer);
    }
}

int ShadersBuffer::GetSlotForTexture(const Mesh* mesh, bool isSpecular)
{
    auto& texturesMap = isSpecular ? m_TexturesSpecular : m_TexturesDiffuse; 
    const auto& textureArray = isSpecular ? mesh->m_TextureSpecular : mesh->m_TextureDiffuse;

    for (const auto& texture : textureArray)
    {
        if (texture && texturesMap.contains(texture->m_FilePath)) 
        {
            return texturesMap[texture->m_FilePath].m_Index;  
        }
    }

    return isSpecular ? static_cast<int>(m_TexturesSpecular.size()) : static_cast<int>(m_TexturesDiffuse.size()); // Retrieve the next available index for the texture
}

void ShadersBuffer::InsertIndices(const Mesh* mesh) 
{
    const int size = static_cast<int>(m_Vertices.size());
    
    m_Indices.reserve(m_Indices.size() + mesh->m_Indices.size());
    
    for (const auto& index : mesh->m_Indices)
    {
        m_Indices.push_back(index + size);
    }
}

void ShadersBuffer::SetModelIndexForVertexBuffers(Mesh* mesh) const
{
    const int index = static_cast<int>(m_Models.size());

    if( index > 0)
    {
        for (auto& vertex : mesh->m_Vertices)
        {
            vertex.m_IndexModel = index;
        } 
    }
}

void ShadersBuffer::BindTexturesToShader(ShaderType, Shader* shader)
{
    shader->Bind();

    SetUniformAndBind( shader, "u_Diffuse", m_Diffuse, m_TexturesDiffuse );
    SetUniformAndBind( shader, "u_Specular", m_Specular, m_TexturesSpecular );
}

void ShadersBuffer::SetUniformAndBind(Shader* shader, const std::string& uniformName, const std::vector<int>& arraySlot, const std::unordered_map<std::string, Texture>& textures) const
{
    const int MaxSlotForTextures = Application::Get()->GetMaxSlotForTextures();
    
    if (!textures.empty()) 
    {
        shader->SetUniform1iv(uniformName, arraySlot); 
    }
    
    for (auto& texture : textures)
    {
        texture.second.Bind( texture.second.m_Slot - MaxSlotForTextures * m_IndexBuffer );  
    }
}
