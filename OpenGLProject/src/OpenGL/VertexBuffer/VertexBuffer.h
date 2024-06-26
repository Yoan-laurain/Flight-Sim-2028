#pragma once

#include "../../Library/coreMinimal.h"
#include <vector>

struct Vertex
{
	Vertex(const Vec3<float>& position, const Vec3<float>& normal, const Vec3<float>& color, const Vec2<float>& texUV);
	explicit Vertex(const Vec3<float>& position);
	Vertex() = default;

	Vec3<float> m_Position;
	Vec3<float> m_Normal;
	Vec3<float> m_Color;
	Vec2<float> m_TexUV;
	
	int m_IndexModel{}; 
	int m_IndexDiffuse{}; 
	int m_IndexSpecular{}; 
};

class VertexBuffer
{
	public:
		explicit VertexBuffer( const std::vector<Vertex>& vertices );
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;
		void SetDatas(const std::vector<Vertex>& vertices);

		std::vector<Vertex> m_Vertices;
	
	private:
		unsigned int m_id;
};