#include "Model.h"
#include "OpenGL/VertexBuffer/VertexBuffer.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include "Core/Application.h"
#include "Library/Math.h"
#include "Mesh/Mesh.h"

Model::Model(const ShaderType shaderType)
	: m_ShaderType(shaderType)
	, m_rotation(Vec3<float>(0))
	, m_translation(Vec3<float>(0))
	, m_scale(Vec3<float>(1))
{
	Application::Get()->m_ModelCount++;
}

Model::Model(const std::vector<Mesh*>& meshes, const ShaderType shaderType)
	: m_ShaderType(shaderType)
	, m_rotation(Vec3(0.0f))
	, m_translation(Vec3(0.0f))
	, m_scale(Vec3(1.0f))
{
	Application::Get()->m_ModelCount++;
	for (const auto mesh : meshes)
	{
		m_Meshes.emplace_back(std::unique_ptr<Mesh>(mesh));
	}

	SendDataRender();
}

Model::~Model() = default;

void Model::Update()
{
	for (const auto& Mesh : m_Meshes)
	{
		Mesh->m_Transform = m_translation;
		Mesh->m_Rotation = Quaternion(m_rotation);
		Mesh->m_Scale = m_scale;
	}

	Application::Get()->GetBatchRenderer()->UpdateModelDatas(this, m_ShaderType);
}

void Model::SendDataRender()
{
	BatchRenderer* batchRenderer = Application::Get()->GetBatchRenderer();
	for (const auto& Mesh : m_Meshes)
	{
		batchRenderer->AddNewMesh(this, Mesh.get());
	}
}

void Model::SetRotation(const Vec3<float>& rotation)
{
	m_rotation = Math::radians(rotation);
	Update();
}

void Model::SetTranslation(const Vec3<float>& translation)
{
	m_translation = translation;
	Update();
}

void Model::SetScale(const Vec3<float>& scale)
{
	m_scale = scale;
	Update();
}

Vec3<float> Model::GetRotation() const
{
	return m_rotation;
}

Vec3<float> Model::GetTranslation() const
{
	return m_translation;
}

Vec3<float> Model::GetScale() const
{
	return m_scale;
}