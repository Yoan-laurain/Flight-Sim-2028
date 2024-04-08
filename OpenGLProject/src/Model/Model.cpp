#include "Model.h"
#include "../OpenGL/VertexBuffer/VertexBuffer.h"
#include "../OpenGL/Mesh/Mesh.h"
#include "../Managers/BatchRenderer.h"
#include "../Core/Application.h"

#include <glm/ext.hpp>
#include <iostream>

Model::Model(const char* file, const ShaderType shaderType)
	: m_ShaderType(shaderType)
	, m_Rotation(glm::vec3(0))
	, m_Translation(glm::vec3(0))
	, m_Scale(glm::vec3(1))
	, m_File(file)
{
}

Model::Model(const std::vector<Mesh*>& meshes, const ShaderType shaderType)
	: m_ShaderType(shaderType)
	, m_Rotation(glm::vec3(0.0f))
	, m_Translation(glm::vec3(0.0f))
	, m_Scale(glm::vec3(1.0f))
	, m_File(nullptr)
{

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	for (const auto mesh : meshes)
	{
		m_Meshes.emplace_back(std::unique_ptr<Mesh>(mesh));
		vertices.insert(vertices.end(), mesh->m_Vertices.begin(), mesh->m_Vertices.end());
		indices.insert(indices.end(), mesh->m_Indices.begin(), mesh->m_Indices.end());
	}

	SendDataRender();
}

Model::~Model() = default;

int Model::GetNumberOfTriangles()
{
	int numTriangles = 0;
	
	for (const auto& Mesh : m_Meshes)
	{
		numTriangles += Mesh->m_TriangleCount;
	}

	return numTriangles;
}

void Model::Update()
{
	for (const auto& Mesh : m_Meshes)
	{
		Mesh->m_Transform = m_Translation;
		Mesh->m_Rotation = glm::quat(glm::radians(m_Rotation));
		Mesh->m_Scale = m_Scale;
	}
}

void Model::SendDataRender()
{
	Application::Get()->GetBatchRenderer()->AddNewMesh(this, m_Meshes.back().get());
}

void Model::SetRotation(const glm::vec3 rotation)
{
	m_Rotation = rotation;
	Update();
}

void Model::SetTranslation(const glm::vec3 translation)
{
	m_Translation = translation;
	Update();
}

void Model::SetScale(const glm::vec3 scale)
{
	m_Scale = scale;
	Update();
}

glm::vec3& Model::GetRotation()
{
	return m_Rotation;
}

glm::vec3& Model::GetTranslation()
{
	return m_Translation;
}

glm::vec3& Model::GetScale()
{
	return m_Scale;
}