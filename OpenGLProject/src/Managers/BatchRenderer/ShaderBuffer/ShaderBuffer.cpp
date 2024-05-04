#include "ShaderBuffer.h"
#include "Core/Application.h"
#include "Library/Math.h"
#include "Model/Model.h"
#include "OpenGL/IndexBuffer/IndexBuffer.h"
#include "Mesh/Mesh.h"
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

    AddTextureToTheMap(m_TexturesDiffuse, mesh->m_TextureDiffuse);
    AddTextureToTheMap(m_TexturesSpecular, mesh->m_TextureSpecular);
}

void ShadersBuffer::SetIndexOfTextures(Mesh* mesh)
{
    const int indexSpecular = GetIndexForTexture(mesh, true);
    const int indexDiffuse = GetIndexForTexture(mesh, false);

    for (auto& vertex : mesh->m_Vertices) 
    {
        vertex.m_IndexSpecular = indexSpecular;
        vertex.m_IndexDiffuse = indexDiffuse;
    }
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
    if ( m_needToRegroupTransformsAgain )
    {
        m_transformsData.clear();

        // Concatenate all the transforms data of all the models of the buffer
        for (const auto& [model, transforms] : m_ModelsTransforms)
        {
            m_transformsData.insert(m_transformsData.end(), transforms.begin(), transforms.end());
        }
        
        m_needToRegroupTransformsAgain = false;
    }
    return m_transformsData;
}

void ShadersBuffer::AddTextureToTheMap(std::unordered_map<std::string, Texture>& textures, const std::vector<std::unique_ptr<Texture>>& texture)
{
    const int size = static_cast<int>(textures.size());
    
    // loop through all the textures of the mesh
    for (const auto& tex : texture)
    {
        // if the texture is not already in the map, add it
        if (tex && !textures.contains(tex->m_FilePath))
        {
            tex->m_Index = size;
            textures[tex->m_FilePath] = *tex;
        }
    }
}

void ShadersBuffer::ExtractModelTransformData(const std::vector<Model*>::value_type& model) 
{
    // Clear the previous data
    if (m_ModelsTransforms.contains(model))
    {
        m_ModelsTransforms[model].clear();
    }

    // loop through all the meshes of the model
    for (const auto& mesh : model->m_Meshes)
    {
        // Create the transform data for the mesh
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

    m_needToRegroupTransformsAgain = true;
}

void ShadersBuffer::UpdateModelVerticesDatas(Model* model)
{
    // Clear the previous data
    if (m_ModelsVertices.contains(model))
    {
        m_ModelsVertices[model].clear();
    }

    // loop through all the meshes of the model
    for (const auto& mesh : model->m_Meshes)
    {
        // Append the vertices of the mesh to the list of vertices of the model if it already exist
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

    // Concatenate all the vertices of all the models of the buffer
    for (const auto& [model, vertices] : m_ModelsVertices)
    {
        m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
    }

    // Update the VBO with the new vertices
    m_VBO->SetDatas(m_Vertices);
}

void ShadersBuffer::InitTextureUniform()
{
    CreateTextureUniforms(m_TexturesDiffuse, m_diffuse);
    CreateTextureUniforms(m_TexturesSpecular, m_specular);
}

void ShadersBuffer::CreateTextureUniforms(const std::unordered_map<std::string, Texture>& texturesMap, std::vector<int>& array)
{
    // clear the previous data
    array.clear();
    
    const int maxSlotForTextures = Application::Get()->GetMaxSlotForTextures();

    // loop through all the textures of the buffer
    for (const auto& texture : texturesMap)
    {
        // Offset the slot by the number of textures already added by the previous buffers
        const int offset = maxSlotForTextures * m_IndexBuffer;
        array.emplace_back(texture.second.m_Slot - offset);
    }
}

int ShadersBuffer::GetIndexForTexture(const Mesh* mesh, const bool isSpecular)
{
    auto& texturesMap = isSpecular ? m_TexturesSpecular : m_TexturesDiffuse; 
    const auto& textureArray = isSpecular ? mesh->m_TextureSpecular : mesh->m_TextureDiffuse;

    // loop through all the textures of the mesh
    for (const auto& texture : textureArray)
    {
        // if the texture already exist, return its index
        if (texture && texturesMap.contains(texture->m_FilePath)) 
        {
            return texturesMap[texture->m_FilePath].m_Index;  
        }
    }

    // Retrieve the next available index for the Texture
    return isSpecular ? static_cast<int>(m_TexturesSpecular.size()) : static_cast<int>(m_TexturesDiffuse.size()); 
}

void ShadersBuffer::InsertIndices(const Mesh* mesh) 
{
    const int size = static_cast<int>(m_Vertices.size());
    
    m_Indices.reserve(m_Indices.size() + mesh->m_Indices.size());

    // Loop through all the indices of the mesh
    for (const auto& index : mesh->m_Indices)
    {
        // We add the offset of the previous indices added by the previous meshes
        m_Indices.emplace_back(index + size);
    }
}

void ShadersBuffer::SetModelIndexForVertexBuffers(Mesh* mesh) const
{
    const int index = static_cast<int>(m_Models.size());

    // Since default value is 0, we only need to set the index if it's greater than 0
    if(index > 0)
    {
        for (auto& vertex : mesh->m_Vertices)
        {
            vertex.m_IndexModel = index;
        } 
    }
}

void ShadersBuffer::SetTexturesUniforms(ShaderType, Shader* shader) const
{
    shader->Bind();

    SetUniformAndBind( shader, "u_Diffuse", m_diffuse, m_TexturesDiffuse );
    SetUniformAndBind( shader, "u_Specular", m_specular, m_TexturesSpecular );
}

void ShadersBuffer::SetUniformAndBind(Shader* shader, const std::string& uniformName, const std::vector<int>& arraySlot, const std::unordered_map<std::string, Texture>& textures) const
{
    const int MaxSlotForTextures = Application::Get()->GetMaxSlotForTextures();
    
    if (!textures.empty()) 
    {
        // loop through all the textures of the buffer and bind them
        for (auto& texture : textures)
        {
            // Offset the slot by the number of textures already added by the previous buffers
            const int offset = MaxSlotForTextures * m_IndexBuffer;
            texture.second.Bind( texture.second.m_Slot - offset );  
        }

        shader->SetUniform1iv(uniformName, arraySlot);
    }
}