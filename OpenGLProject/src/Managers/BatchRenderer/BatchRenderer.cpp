﻿#include "BatchRenderer.h"
#include "../../OpenGL/VertexBuffer/VertexBuffer.h" // Ne pas delete
#include "../../OpenGL/VertexBufferLayout/VertexBufferLayout.h"
#include "../../OpenGL/Shader/Shader.h"
#include "../../OpenGL/Mesh/Mesh.h"
#include "../../OpenGL/Textures/Texture.h" 
#include "../ShaderManager/ShaderManager.h"
#include "../../Core/Application.h"
#include "../../Renderer/Renderer.h"
#include "../../Model/Model.h"
#include "../../Config.h"
#include "OpenGL/IndexBuffer/IndexBuffer.h" // Ne pas delete
#include "ShaderBuffer\ShaderBuffer.h"
#include "OpenGL/ShaderStorageBuffer/ShaderStorageBufferObject.h"
#include "OpenGL/VertexArray/VertexArray.h"

#include <iostream>

BatchRenderer::BatchRenderer() 
{
    m_Layout = std::make_unique<VertexBufferLayout>();
    m_Layout->Push<float>(3); // m_Position
    m_Layout->Push<float>(3); // Normal
    m_Layout->Push<float>(3); // Colors
    m_Layout->Push<float>(2); // Texture coordinates;
    m_Layout->Push<float>(1); // index;
    m_Layout->Push<float>(1); // index diffuse
    m_Layout->Push<float>(1); // index specular
}

BatchRenderer::~BatchRenderer() = default;

void BatchRenderer::AddNewMesh(Model* model, Mesh* mesh)
{
    const bool bNewBufferCreated = CreateNewBufferIfNeed(model->m_ShaderType, mesh,model);

    if (!bNewBufferCreated) 
    {
        m_ShadersData[model->m_ShaderType].back()->AddNewMesh(mesh, model);
    }
}

int BatchRenderer::Draw()
{
    int drawCalls = 0;
    
    for (auto& shaderData : m_ShadersData)
    {
		Shader* shader = Application::Get()->GetShaderManager()->GetShader(shaderData.first);
        
        for (const auto& currentBuffer : m_ShadersData[shaderData.first])
        {
            if (currentBuffer->m_VBO == nullptr) // If the buffer is not initialized, initialize it
            {
               currentBuffer->Init();
            }

            currentBuffer->m_SSBO->SetData( currentBuffer->transformsDatas.data(), sizeof(TransformData) * currentBuffer->transformsDatas.size(), 7);
            currentBuffer->BindTexturesToShader(shaderData.first, shader);
       
            currentBuffer->m_VAO->Bind(); 
            currentBuffer->m_VAO->AddBuffer(*currentBuffer->m_VBO, *m_Layout);
        
            shader->OnBeforeDraw();
            Application::Get()->GetRenderer()->Draw( *currentBuffer->m_VAO, *currentBuffer->m_IBO, *shader);
            shader->OnAfterDraw();
        
            drawCalls++;
        }
	}
    
    return drawCalls;
}

Texture* BatchRenderer::CreateOrGetTexture(const char* image, const char* texType, const ShaderType shaderType)
{
    if (m_ShadersData.contains(shaderType))
    {
        const auto& shadersData = m_ShadersData[shaderType].back();

        if (texType == Diffuse && shadersData->m_TexturesDiffuse.contains(image))
        {
            return new Texture(shadersData->m_TexturesDiffuse[image]);
        }
        
        if (texType == Specular && shadersData->m_TexturesSpecular.contains(image))
        {
            return new Texture(shadersData->m_TexturesSpecular[image]);
        }
    }

    return new Texture(image, texType, shaderType);
}

int BatchRenderer::GetNextIndexToBindTextureTo(const ShaderType shaderType)
{
    const int index = m_IndexTextures[shaderType];
    m_IndexTextures[shaderType]++;
    return index;
}

bool BatchRenderer::CreateNewBufferIfNeed(const ShaderType shaderType, Mesh* mesh,Model* model)
{
    if (!m_ShadersData.contains(shaderType))  // Buffer does not already exist for this shader
    {
        m_ShadersData[shaderType].emplace_back(std::make_unique<ShadersBuffer>(ShadersBuffer(mesh, model, 0)));
        return true;
    }

    // clamp to 1 min to avoid division by 0
    const auto size = !m_ShadersData[shaderType].empty() ? m_ShadersData[shaderType].size() : 1;

    // if the number of textures is greater than the max slot for textures, create a new buffer
    for (const auto& texture : mesh->m_Textures)
    {
        const int index = texture.m_Slot - Application::Get()->GetMaxSlotForTextures() * size;
        if ( index >= 0 )
        {
            m_ShadersData[shaderType].emplace_back(std::make_unique<ShadersBuffer>(ShadersBuffer(mesh, model, m_ShadersData[shaderType].size())));
            return true;
        }
    }

    return false;
}