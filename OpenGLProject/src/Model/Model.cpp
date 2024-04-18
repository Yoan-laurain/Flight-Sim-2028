#include "Model.h"
#include "../OpenGL/VertexBuffer/VertexBuffer.h"
#include "../OpenGL/Mesh/Mesh.h"
#include "../Managers/BatchRenderer/BatchRenderer.h"
#include "../Core/Application.h"
#include "Library/Math.h"

Model::Model(const char* file, const ShaderType shaderType)
	: m_ShaderType(shaderType)
	, m_Rotation(Vec3<float>(0))
	, m_Translation(Vec3<float>(0))
	, m_Scale(Vec3<float>(1))
{
}

Model::Model(const std::vector<Mesh*>& meshes, const ShaderType shaderType)
	: m_ShaderType(shaderType)
	, m_Rotation(Vec3(0.0f))
	, m_Translation(Vec3(0.0f))
	, m_Scale(Vec3(1.0f))
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

int Model::GetNumberOfTriangles() const
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
		Mesh->m_Rotation = Quaternion(m_Rotation);
		Mesh->m_Scale = m_Scale;
	}

	Application::Get()->GetBatchRenderer()->UpdateModelDatas(this, m_ShaderType);
}

void Model::SendDataRender()
{
	for (const auto& Mesh : m_Meshes)
	{
		Application::Get()->GetBatchRenderer()->AddNewMesh(this, Mesh.get());
	}
}

void Model::SetRotation(const Vec3<float>& rotation)
{
	m_Rotation = Math::radians(rotation);
	Update();
}

void Model::SetTranslation(const Vec3<float>& translation)
{
	m_Translation = translation;
	Update();
}

void Model::SetScale(const Vec3<float>& scale)
{
	m_Scale = scale;
	Update();
}

Vec3<float> Model::GetRotation() const
{
	return m_Rotation;
}

Vec3<float> Model::GetTranslation() const
{
	return m_Translation;
}

Vec3<float> Model::GetScale() const
{
	return m_Scale;
}