#include "ModelLoader.h"
#include "OpenGL/Textures/Texture.h"
#include "Config.h"
#include "Core/Application.h"
#include "Managers/BatchRenderer/BatchRenderer.h"
#include "OpenGL/VertexBuffer/VertexBuffer.h"
#include "Model/Model.h"
#include "Library/Math.h"
#include "Mesh/Mesh.h"

#include <fstream>

#define JSON_FIELD_NODES "nodes"
#define JSON_FIELD_MESHES "meshes"
#define JSON_FIELD_MESH "mesh"
#define JSON_FIELD_PRIMITIVES "primitives"
#define JSON_FIELD_ATTRIBUTES "attributes"
#define JSON_FIELD_INDICES "indices"
#define JSON_FIELD_ACCESSORS "accessors"
#define JSON_FIELD_SCALE "scale"
#define JSON_FIELD_TRANSLATION "translation"
#define JSON_FIELD_ROTATION "rotation"
#define JSON_FIELD_MATRIX "matrix"
#define JSON_FIELD_CHILDREN "children"
#define JSON_FIELD_BUFFERVIEW "bufferView"
#define JSON_FIELD_BUFFERVIEWS "bufferViews"
#define JSON_FIELD_BYTEOFFSET "byteOffset"
#define JSON_FIELD_COUNT "count"
#define JSON_FIELD_TYPE "type"
#define JSON_FIELD_COMPONENTTYPE "componentType"
#define JSON_FIELD_IMAGES "images"
#define JSON_FIELD_URI "uri"
#define JSON_FIELD_METALLICROUGHNESS "metallicRoughness"
#define JSON_FIELD_SPECULAR "specular"
#define JSON_FIELD_BASECOLOR "baseColor"
#define JSON_FIELD_DIFFUSE "diffuse"
#define JSON_FIELD_SCALAR "SCALAR"
#define JSON_FIELD_VEC2 "VEC2"
#define JSON_FIELD_VEC3 "VEC3"
#define JSON_FIELD_VEC4 "VEC4"

std::string Get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary); // Open the file in binary mode
	
	if (in)
	{
		std::string contents;
		
		in.seekg(0, std::ios::end); // Seek to the end of the file
		contents.resize(in.tellg()); // Resize the string to the size of the file
		in.seekg(0, std::ios::beg); // Seek back to the beginning of the file
		in.read(&contents[0], static_cast<long long>(contents.size())); // Read the file into the string
		in.close(); // Close the file
		
		return contents;
	}
	
	throw errno;
}

ModelLoader::ModelLoader(const char* file, const ShaderType shaderType)
	: m_Model(nullptr)
	, m_ShaderType(shaderType)
	, m_file(file)
{
	m_Model = std::make_unique<Model>(shaderType);
	
	std::string text = Get_file_contents(file);
	m_json = json::parse(text);

	m_data = GetData();
	
	TraverseNode(0);

	m_Model->SendDataRender();
}

ModelLoader::~ModelLoader() = default;

void ModelLoader::LoadMesh(const unsigned int indMesh)
{
	// Get all accessor indices
	const unsigned int posAccInd = m_json[JSON_FIELD_MESHES][indMesh][JSON_FIELD_PRIMITIVES][0][JSON_FIELD_ATTRIBUTES]["POSITION"];
	const unsigned int normalAccInd = m_json[JSON_FIELD_MESHES][indMesh][JSON_FIELD_PRIMITIVES][0][JSON_FIELD_ATTRIBUTES]["NORMAL"];
	const unsigned int texAccInd = m_json[JSON_FIELD_MESHES][indMesh][JSON_FIELD_PRIMITIVES][0][JSON_FIELD_ATTRIBUTES]["TEXCOORD_0"];
	const unsigned int indAccInd = m_json[JSON_FIELD_MESHES][indMesh][JSON_FIELD_PRIMITIVES][0][JSON_FIELD_INDICES];

	// Use accessor indices to get all vertices components
	const std::vector<float> posVec = GetFloats(m_json[JSON_FIELD_ACCESSORS][posAccInd]);
	const std::vector<Vec3<float>> positions = GroupFloatsVec3(posVec);
	const std::vector<float> normalVec = GetFloats(m_json[JSON_FIELD_ACCESSORS][normalAccInd]);
	const std::vector<Vec3<float>> normals = GroupFloatsVec3(normalVec);
	const std::vector<float> texVec = GetFloats(m_json[JSON_FIELD_ACCESSORS][texAccInd]);
	const std::vector<Vec2<float>> texUVs = GroupFloatsVec2(texVec);
	
	// Combine all the vertex components and also get the indices and textures
	std::vector<Vertex> vertices = AssembleVertices(positions, normals, texUVs);
	std::vector<unsigned> indices = GetIndices(m_json[JSON_FIELD_ACCESSORS][indAccInd]);
	std::vector<Texture> textures = GetTextures();
	
	 // If there is no Texture set a default Texture
	 if (textures.empty())
	 {
		 Texture* defaultTex = Application::Get()->GetBatchRenderer()->CreateOrGetTexture(DEFAULT_TEXTURE_PATH, Diffuse, m_ShaderType);
	 	 textures.emplace_back(*defaultTex);

	 	 delete defaultTex;
	 }
	
	m_Model->m_Meshes.emplace_back(std::make_unique<Mesh>(vertices, indices, textures, m_matricesMeshes.back()));
}

void ModelLoader::TraverseNode(const unsigned int nextNode, const Mat4<float>& matrix)
{
	// Current node
	json node = m_json[JSON_FIELD_NODES][nextNode];

	// Get translation if it exists
	Vec3<float> translation = Vec3(0.0f, 0.0f, 0.0f);
	if (node.find(JSON_FIELD_TRANSLATION) != node.end())
	{
		float transValues[3];
		for (unsigned int i = 0; i < node[JSON_FIELD_TRANSLATION].size(); i++)
			transValues[i] = (node[JSON_FIELD_TRANSLATION][i]);
		translation = Math::make_vec3(transValues);
	}
	// Get quaternion if it exists
	Quaternion rotation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find(JSON_FIELD_ROTATION) != node.end())
	{
		float rotValues[4] =
		{
			node[JSON_FIELD_ROTATION][3],
			node[JSON_FIELD_ROTATION][0],
			node[JSON_FIELD_ROTATION][1],
			node[JSON_FIELD_ROTATION][2]
		};
		rotation = Math::make_quaternion(rotValues);
	}
	// Get scale if it exists
	Vec3 scale(1.0f, 1.0f, 1.0f);
	if (node.find(JSON_FIELD_SCALE) != node.end())
	{
		float scaleValues[3];
		for (unsigned int i = 0; i < node[JSON_FIELD_SCALE].size(); i++)
			scaleValues[i] = (node[JSON_FIELD_SCALE][i]);
		scale = Math::make_vec3(scaleValues);
	}
	// Get matrix if it exists
	auto matNode = Mat4(1.0f);
	if (node.find(JSON_FIELD_MATRIX) != node.end())
	{
		float matValues[16];
		for (unsigned int i = 0; i < node[JSON_FIELD_MATRIX].size(); i++)
			matValues[i] = (node[JSON_FIELD_MATRIX][i]);
		matNode = Math::make_mat4(matValues);
	}

	const Mat4<float> trans = Math::translate(Mat4(1.0f), translation);
	const Mat4<float> rot = Math::mat4_cast(rotation);
	const Mat4<float> sca = Math::Scale(Mat4(1.0f), scale);

	const Mat4<float> matNextNode = matrix * matNode * trans * rot * sca;

	// Check if the node contains a mesh and if it does load it
	if (node.find(JSON_FIELD_MESH) != node.end())
	{
		m_translationsMeshes.push_back(translation);
		m_rotationsMeshes.push_back(rotation);
		m_scalesMeshes.push_back(scale);
		m_matricesMeshes.push_back(matNextNode);

		LoadMesh(node[JSON_FIELD_MESH]);
	}

	// Check if the node has children, and if it does, apply this function to them with the matNextNode
	if (node.find(JSON_FIELD_CHILDREN) != node.end())
	{
		for (auto& i : node[JSON_FIELD_CHILDREN])
			TraverseNode(i, matNextNode);
	}
}

std::vector<unsigned char> ModelLoader::GetData()
{
	// Create a place to store the raw text, and get the uri of the .bin file
	const std::string uri = m_json["buffers"][0]["uri"];

	// Store raw text data into bytesText
	const std::string fileStr = std::string(m_file);
	const std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
	std::string bytesText = Get_file_contents((fileDirectory + uri).c_str());

	// Transform the raw text data into bytes and put them in a vector
	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;
}

std::vector<float> ModelLoader::GetFloats(json accessor)
{
	std::vector<float> floatVec;
	
	// Get properties from the accessor
	const unsigned int buffViewInd = accessor.value( JSON_FIELD_BUFFERVIEW, 1);
	const unsigned int count = accessor[JSON_FIELD_COUNT];
	const unsigned int accByteOffset = accessor.value(JSON_FIELD_BYTEOFFSET, 0);
	const std::string type = accessor[JSON_FIELD_TYPE];

	// Get properties from the bufferView
	const json bufferView = m_json[JSON_FIELD_BUFFERVIEWS][buffViewInd];
	const unsigned int byteOffset = bufferView.value(JSON_FIELD_BYTEOFFSET, 0);
	
	// Interpret the type and store it into numPerVert
	unsigned int numPerVert;
	if (type == JSON_FIELD_SCALAR) numPerVert = 1;
	else if (type == JSON_FIELD_VEC2) numPerVert = 2;
	else if (type == JSON_FIELD_VEC3) numPerVert = 3;
	else if (type == JSON_FIELD_VEC4) numPerVert = 4;
	else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

	// Go over all the bytes in the data at the correct place using the properties from above
	const unsigned int beginningOfData = byteOffset + accByteOffset;
	const unsigned int lengthOfData = count * 4 * numPerVert;
	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData;)
	{
		const unsigned char bytes[] = { m_data[i++], m_data[i++], m_data[i++], m_data[i++] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}

	return floatVec;
}

std::vector<unsigned> ModelLoader::GetIndices(json accessor) 
{
	std::vector<unsigned> indices;

	// Get properties from the accessor
	const unsigned int buffViewInd = accessor.value(JSON_FIELD_BUFFERVIEW, 0);
	const unsigned int count = accessor[JSON_FIELD_COUNT];
	const unsigned int accByteOffset = accessor.value(JSON_FIELD_BYTEOFFSET, 0);
	const unsigned int componentType = accessor[JSON_FIELD_COMPONENTTYPE];

	// Get properties from the bufferView
	const json bufferView = m_json[JSON_FIELD_BUFFERVIEWS][buffViewInd];
	const unsigned int byteOffset = bufferView.value(JSON_FIELD_BYTEOFFSET, 0);

	// Get indices with regards to their type: unsigned int, unsigned short, or short
	const unsigned int beginningOfData = byteOffset + accByteOffset;
	if (componentType == 5125)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4;)
		{
			const unsigned char bytes[] = { m_data[i++], m_data[i++], m_data[i++], m_data[i++] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back(value);
		}
	}
	else if (componentType == 5123)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2;)
		{
			const unsigned char bytes[] = { m_data[i++], m_data[i++] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back(value);
		}
	}
	else if (componentType == 5122)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; )
		{
			const unsigned char bytes[] = { m_data[i++], m_data[i++] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back(value);
		}
	}

	return indices;
}

std::vector<Texture> ModelLoader::GetTextures()
{
	std::vector<Texture> textures;

	const std::string fileStr = std::string(m_file);
	const std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	// Go over all images
	for (unsigned int i = 0; i < m_json[JSON_FIELD_IMAGES].size(); i++)
	{
		// uri of current Texture
		std::string texPath = m_json[JSON_FIELD_IMAGES][i][JSON_FIELD_URI];

		// Check if the Texture has already been loaded
		bool skip = false;
		for (unsigned int j = 0; j < m_loadedTexName.size(); j++)
		{
			if (m_loadedTexName[j] == texPath)
			{
				textures.push_back(m_loadedTex[j]);
				skip = true;
				break;
			}
		}
		
		// If the Texture has been loaded, skip this
		if (!skip)
		{
			// Load diffuse Texture
			if (texPath.find(JSON_FIELD_BASECOLOR) != std::string::npos || texPath.find(JSON_FIELD_DIFFUSE) != std::string::npos)
			{
				const Texture* defaultTex = Application::Get()->GetBatchRenderer()->CreateOrGetTexture((fileDirectory + texPath).c_str(), Diffuse, m_ShaderType);
				textures.push_back(*defaultTex);
				m_loadedTex.push_back(*defaultTex);
				m_loadedTexName.push_back(texPath);
				
				delete defaultTex;
			
			}
			// Load specular Texture
			else if (texPath.find(JSON_FIELD_METALLICROUGHNESS) != std::string::npos || texPath.find(JSON_FIELD_SPECULAR) != std::string::npos)
			{
				const Texture* defaultTex = Application::Get()->GetBatchRenderer()->CreateOrGetTexture((fileDirectory + texPath).c_str(), Specular, m_ShaderType);
				textures.push_back(*defaultTex);
				m_loadedTex.push_back(*defaultTex);
				m_loadedTexName.push_back(texPath);
				delete defaultTex;
			}
		}
	}

	return textures;
}

std::vector<Vertex> ModelLoader::AssembleVertices(const std::vector<Vec3<float>>& positions, const std::vector<Vec3<float>>& normals, const std::vector<Vec2<float>>& texUVs )
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < static_cast<int>(positions.size()); i++)
	{
		vertices.emplace_back(positions[i], normals[i], Vec3(1.0f, 1.0f, 1.0f), texUVs[i]);
	}
	return vertices;
}

std::vector<Vec2<float>> ModelLoader::GroupFloatsVec2(const std::vector<float>& floatVec)
{
	std::vector<Vec2<float>> vectors;
	for (int i = 0; i < static_cast<int>(floatVec.size());)
	{
		vectors.emplace_back(floatVec[i++], floatVec[i++]);
	}
	return vectors;
}

std::vector<Vec3<float>> ModelLoader::GroupFloatsVec3(const std::vector<float>& floatVec)
{
	std::vector<Vec3<float>> vectors;
	for (int i = 0; i < static_cast<int>(floatVec.size());)
	{
		vectors.emplace_back(floatVec[i++], floatVec[i++], floatVec[i++]);
		//i+=3;
	}
	return vectors;
}

std::vector<Vec4<float>> ModelLoader::GroupFloatsVec4(const std::vector<float>& floatVec)
{
	std::vector<Vec4<float>> vectors;
	for (int i = 0; i < static_cast<int>(floatVec.size()); )
	{
		vectors.emplace_back(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]);
		//i+=4;
	}
	return vectors;
}