#include "BatchRenderer.h"
#include "OpenGL/VertexBuffer/VertexBuffer.h" // Ne pas delete
#include "OpenGL/VertexBufferLayout/VertexBufferLayout.h"
#include "OpenGL/Shader/Shader.h"
#include "Mesh/Mesh.h"
#include "OpenGL/Textures/Texture.h" 
#include "../ShaderManager/ShaderManager.h"
#include "Core/Application.h"
#include "Renderer/Renderer.h"
#include "Model/Model.h"
#include "Config.h"
#include "OpenGL/IndexBuffer/IndexBuffer.h" // Ne pas delete
#include "ShaderBuffer\ShaderBuffer.h"
#include "OpenGL/ShaderStorageBuffer/ShaderStorageBufferObject.h"
#include "OpenGL/VertexArray/VertexArray.h"

#include <iostream>

BatchRenderer::BatchRenderer() 
{
    m_layout = std::make_unique<VertexBufferLayout>();
    m_layout->Push<float>(3); // m_Position
    m_layout->Push<float>(3); // Normal
    m_layout->Push<float>(3); // Colors
    m_layout->Push<float>(2); // Texture coordinates;
    m_layout->Push<float>(1); // index;
    m_layout->Push<float>(1); // index diffuse
    m_layout->Push<float>(1); // index specular
}

BatchRenderer::~BatchRenderer() = default;

void BatchRenderer::AddNewMesh(Model* model, Mesh* mesh)
{
    if (!CreateNewBufferIfNeed(model->m_ShaderType, mesh,model)) 
    {
        AddToAppropriateBuffer(model, mesh);
    }
}

int BatchRenderer::Draw()
{
    int drawCalls = 0;
    
    for (auto& shaderData : m_shadersData)
    {
		Shader* shader = Application::Get()->GetShaderManager()->GetShader(shaderData.first);

        // We loop through all the buffers for each shader
        for (const auto& currentBuffer : m_shadersData[shaderData.first])
        {
            if (currentBuffer->m_VBO == nullptr) // If the buffer is not initialized, initialize it
            {
               currentBuffer->Init();
            }

            // Update the SSBO with the new data
            currentBuffer->m_SSBO->SetData(currentBuffer->GetAllTransformsData().data(), sizeof(TransformData) * static_cast<unsigned int>(currentBuffer->GetAllTransformsData().size()), 7);
            currentBuffer->SetTexturesUniforms(shaderData.first, shader);
       
            currentBuffer->m_VAO->Bind(); 
            currentBuffer->m_VAO->AddBuffer(*currentBuffer->m_VBO, *m_layout);
        
            shader->OnBeforeDraw();
            Application::Get()->GetRenderer()->Draw(*currentBuffer->m_VAO, *currentBuffer->m_IBO, *shader);
            shader->OnAfterDraw();
        
            drawCalls++;
        }
	}
    
    return drawCalls;
}

Texture* BatchRenderer::CreateOrGetTexture(const char* image, const char* texType, const ShaderType shaderType)
{
    // if the shader does not have any buffer yet, create a new texture
    if (!m_shadersData.contains(shaderType))
        return new Texture(image, texType, shaderType);

    const auto& shaderTextures = m_shadersData[shaderType];

    // loop through all the buffers for the shader
    for (const auto& arrayTexture : shaderTextures)
    {
        const auto& texturesMap = (texType == Diffuse) ? arrayTexture->m_TexturesDiffuse : arrayTexture->m_TexturesSpecular;

        // if the texture already exist, return it
        auto textureIt = texturesMap.find(image);
        if (textureIt != texturesMap.end())
            return new Texture(textureIt->second);
    }

    // if the texture does not exist, create a new one
    return new Texture(image, texType, shaderType);
}

void BatchRenderer::UpdateModelData(Model* model, ShaderType shaderType, UpdateType updateType)
{
    // Loop through all the buffers for the shader
    for (const auto& shaderData : m_shadersData[shaderType])
    {
        // If the model is in the buffer, update the data
        if (shaderData->m_ModelsTransforms.contains(model))
        {
            switch (updateType)
            {
                case UpdateType::Transforms:
                    shaderData->ExtractModelTransformData(model);
                    break;
                case UpdateType::Vertices:
                    shaderData->UpdateModelVerticesDatas(model);
                    break;
            }
            return;
        }
    }
}

int BatchRenderer::GetNextIndexToBindTextureTo(const ShaderType shaderType)
{
    const int index = m_indexTextures[shaderType];
    m_indexTextures[shaderType]++;
    return index;
}

bool BatchRenderer::CreateNewBufferIfNeed(const ShaderType shaderType, Mesh* mesh,Model* model)
{
    if (!m_shadersData.contains(shaderType))  // Buffer does not already exist for this shader
    {
        m_shadersData[shaderType].emplace_back(std::make_unique<ShadersBuffer>(ShadersBuffer(mesh, model, 0)));
        return true;
    }

    // clamp to 1 min to avoid division by 0
    const auto size = !m_shadersData[shaderType].empty() ? m_shadersData[shaderType].size() : 1;

    // loop through all the textures of the mesh
    for (const auto& texture : mesh->m_Textures)
    {
        const int index = texture.m_Slot - Application::Get()->GetMaxSlotForTextures() * static_cast<int>(size);

        // if the number of textures is greater than the max slot for textures, create a new buffer
        if ( index >= 0 )
        {
            m_shadersData[shaderType].emplace_back(std::make_unique<ShadersBuffer>(ShadersBuffer(mesh, model, static_cast<int>(m_shadersData[shaderType].size()))));
            return true;
        }
    }

    return false;
}

void BatchRenderer::AddToAppropriateBuffer(Model* model, Mesh* mesh)
{
    if (!AddToBufferIfAlreadyExisting(model, mesh))
    {
        AddToFirstBufferWhoCanHoldIt(model, mesh);
    }
}

bool BatchRenderer::AddToBufferIfAlreadyExisting(Model* model, Mesh* mesh)
{
    const ShaderType shaderType = model->m_ShaderType;

    // If the shader does not have any buffer yet, return false
    if (m_shadersData[shaderType].size() <= 1)
        return false;

    // Loop through all the buffers for the shader
    for (const auto& array : m_shadersData[shaderType])
    {
        if (IsMeshAlreadyInBuffer(array, mesh))
        {
            array->AddNewMesh(mesh, model);
            return true;
        }
    }

    return false;
}

bool BatchRenderer::IsMeshAlreadyInBuffer(const std::unique_ptr<ShadersBuffer>& array, const Mesh* mesh) const
{
    // Loop through all the textures of the mesh
    for (const auto& texture : mesh->m_Textures)
    {
        const auto& texturesMap = (texture.m_Type == Diffuse) ? array->m_TexturesDiffuse : array->m_TexturesSpecular;

        // If the texture is not in the buffer, return false
        if (!texturesMap.contains(texture.m_FilePath))
            return false;
    }
    return true;
}

void BatchRenderer::AddToFirstBufferWhoCanHoldIt(Model* model, Mesh* mesh)
{
    // Get the number of textures slot required for the mesh
    const int nbTextureSlotRequired = static_cast<int>(mesh->m_Textures.size());

    // Loop through all the buffers for the shader
    for (const auto& array : m_shadersData[model->m_ShaderType])
    {
        const int nbSlotAvailable = Application::Get()->GetMaxSlotForTextures() - static_cast<int>(array->m_TexturesDiffuse.size()) - static_cast<int>(array->m_TexturesSpecular.size());

        // If the buffer can hold the mesh, add it
        if ( nbTextureSlotRequired <= nbSlotAvailable )
        {
            array->AddNewMesh(mesh, model); 
            break;
        }
    }
}