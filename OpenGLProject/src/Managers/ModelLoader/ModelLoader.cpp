#include "ModelLoader.h"
#include "../../OpenGL/Textures/Texture.h"
#include "../../OpenGL/Mesh/Mesh.h"
#include "../../Config.h"
#include "../../Core/Application.h"
#include "../BatchRenderer/BatchRenderer.h"
#include "../../OpenGL/VertexBuffer/VertexBuffer.h"
#include "../../Model/Model.h"
#include "Library/Math.h"

#include <fstream>

std::string Get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary); // Open the file in binary mode
	
	if (in)
	{
		std::string contents;
		
		in.seekg(0, std::ios::end); // Seek to the end of the file
		contents.resize(in.tellg()); // Resize the string to the size of the file
		in.seekg(0, std::ios::beg); // Seek back to the beginning of the file
		in.read(&contents[0], contents.size()); // Read the file into the string
		in.close(); // Close the file
		
		return contents;
	}
	
	throw errno;
}

ModelLoader::ModelLoader(const char* file, const ShaderType shaderType)
	: m_Model(nullptr)
	, m_ShaderType(shaderType)
	, m_File(file)
{
	m_Model = std::make_unique<Model>(file, shaderType);
	
	// Make a JSON object
	std::string text = Get_file_contents(file);
	m_JSON = json::parse(text);

	m_Data = GetData();

	// Traverse all nodes
	TraverseNode(0);

	m_Model->SendDataRender();
}

ModelLoader::~ModelLoader() = default;

void ModelLoader::LoadMesh(const unsigned int indMesh)
{
	// Get all accessor indices
	const unsigned int posAccInd = m_JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
	const unsigned int normalAccInd = m_JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
	const unsigned int texAccInd = m_JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
	const unsigned int indAccInd = m_JSON["meshes"][indMesh]["primitives"][0]["indices"];

	// Use accessor indices to get all vertices components
	const std::vector<float> posVec = GetFloats(m_JSON["accessors"][posAccInd]);
	const std::vector<Vec3<float>> positions = GroupFloatsVec3(posVec);
	const std::vector<float> normalVec = GetFloats(m_JSON["accessors"][normalAccInd]);
	const std::vector<Vec3<float>> normals = GroupFloatsVec3(normalVec);
	const std::vector<float> texVec = GetFloats(m_JSON["accessors"][texAccInd]);
	const std::vector<Vec2<float>> texUVs = GroupFloatsVec2(texVec);
	
	// Combine all the vertex components and also get the indices and textures
	std::vector<Vertex> vertices = AssembleVertices(positions, normals, texUVs);
	std::vector<GLuint> indices = GetIndices(m_JSON["accessors"][indAccInd]);
	std::vector<Texture> textures = GetTextures();
	
	 // If there is no texture set a default texture
	 if (textures.empty())
	 {
		 Texture* defaultTex = Application::Get()->GetBatchRenderer()->CreateOrGetTexture("res/textures/default.jpg", Diffuse, m_ShaderType);
	 	 textures.emplace_back(*defaultTex);

	 	 delete defaultTex;
	 }
	
	m_Model->m_Meshes.emplace_back(std::make_unique<Mesh>(vertices, indices, textures, m_MatricesMeshes.back()));
}

void ModelLoader::TraverseNode(const unsigned int nextNode, const Mat4<float>& matrix)
{
	// Current node
	json node = m_JSON["nodes"][nextNode];

	// Get translation if it exists
	Vec3<float> translation = Vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end())
	{
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			transValues[i] = (node["translation"][i]);
		translation = Math::make_vec3(transValues);
	}
	// Get quaternion if it exists
	Quaternion rotation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end())
	{
		float rotValues[4] =
		{
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = Math::make_quaternion(rotValues);
	}
	// Get scale if it exists
	Vec3<float> scale(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end())
	{
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = (node["scale"][i]);
		scale = Math::make_vec3(scaleValues);
	}
	// Get matrix if it exists
	auto matNode = Mat4(1.0f);
	if (node.find("matrix") != node.end())
	{
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matValues[i] = (node["matrix"][i]);
		matNode = Math::make_mat4(matValues);
	}

	const Mat4<float> trans = Math::translate(Mat4<float>(1.0f), translation);
	const Mat4<float> rot = Math::mat4_cast(rotation);
	const Mat4<float> sca = Math::Scale(Mat4(1.0f), scale);

	const Mat4<float> matNextNode = matrix * matNode * trans * rot * sca;

	// Check if the node contains a mesh and if it does load it
	if (node.find("mesh") != node.end())
	{
		m_TranslationsMeshes.push_back(translation);
		m_RotationsMeshes.push_back(rotation);
		m_ScalesMeshes.push_back(scale);
		m_MatricesMeshes.push_back(matNextNode);

		LoadMesh(node["mesh"]);
	}

	// Check if the node has children, and if it does, apply this function to them with the matNextNode
	if (node.find("children") != node.end())
	{
		for (auto& i : node["children"])
			TraverseNode(i, matNextNode);
	}
}

std::vector<unsigned char> ModelLoader::GetData()
{
	// Create a place to store the raw text, and get the uri of the .bin file
	const std::string uri = m_JSON["buffers"][0]["uri"];

	// Store raw text data into bytesText
	const std::string fileStr = std::string(m_File);
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
	const unsigned int buffViewInd = accessor.value("bufferView", 1);
	const unsigned int count = accessor["count"];
	const unsigned int accByteOffset = accessor.value("byteOffset", 0);
	const std::string type = accessor["type"];

	// Get properties from the bufferView
	const json bufferView = m_JSON["bufferViews"][buffViewInd];
	const unsigned int byteOffset = bufferView.value("byteOffset", 0);

	// Interpret the type and store it into numPerVert
	unsigned int numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

	// Go over all the bytes in the data at the correct place using the properties from above
	const unsigned int beginningOfData = byteOffset + accByteOffset;
	const unsigned int lengthOfData = count * 4 * numPerVert;
	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData;)
	{
		const unsigned char bytes[] = { m_Data[i++], m_Data[i++], m_Data[i++], m_Data[i++] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}

	return floatVec;
}

std::vector<GLuint> ModelLoader::GetIndices(json accessor) 
{
	std::vector<GLuint> indices;

	// Get properties from the accessor
	const unsigned int buffViewInd = accessor.value("bufferView", 0);
	const unsigned int count = accessor["count"];
	const unsigned int accByteOffset = accessor.value("byteOffset", 0);
	const unsigned int componentType = accessor["componentType"];

	// Get properties from the bufferView
	const json bufferView = m_JSON["bufferViews"][buffViewInd];
	const unsigned int byteOffset = bufferView.value("byteOffset", 0);

	// Get indices with regards to their type: unsigned int, unsigned short, or short
	const unsigned int beginningOfData = byteOffset + accByteOffset;
	if (componentType == 5125)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4;)
		{
			const unsigned char bytes[] = { m_Data[i++], m_Data[i++], m_Data[i++], m_Data[i++] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back(value);
		}
	}
	else if (componentType == 5123)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2;)
		{
			const unsigned char bytes[] = { m_Data[i++], m_Data[i++] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back(value);
		}
	}
	else if (componentType == 5122)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; )
		{
			const unsigned char bytes[] = { m_Data[i++], m_Data[i++] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back(static_cast<GLuint>(value));
		}
	}

	return indices;
}

std::vector<Texture> ModelLoader::GetTextures()
{
	std::vector<Texture> textures;

	const std::string fileStr = std::string(m_File);
	const std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	// Go over all images
	for (unsigned int i = 0; i < m_JSON["images"].size(); i++)
	{
		// uri of current texture
		std::string texPath = m_JSON["images"][i]["uri"];

		// Check if the texture has already been loaded
		bool skip = false;
		for (unsigned int j = 0; j < m_LoadedTexName.size(); j++)
		{
			if (m_LoadedTexName[j] == texPath)
			{
				textures.push_back(m_LoadedTex[j]);
				skip = true;
				break;
			}
		}

		// If the texture has been loaded, skip this
		if (!skip)
		{
			// Load diffuse texture
			if (texPath.find("baseColor") != std::string::npos || texPath.find("diffuse") != std::string::npos)
			{
				const Texture* defaultTex = Application::Get()->GetBatchRenderer()->CreateOrGetTexture((fileDirectory + texPath).c_str(), Diffuse, m_ShaderType);
				textures.push_back(*defaultTex);
				m_LoadedTex.push_back(*defaultTex);
				m_LoadedTexName.push_back(texPath);
				
				delete defaultTex;
			
			}
			// Load specular texture
			else if (texPath.find("metallicRoughness") != std::string::npos || texPath.find("specular") != std::string::npos)
			{
				const Texture* defaultTex = Application::Get()->GetBatchRenderer()->CreateOrGetTexture((fileDirectory + texPath).c_str(), Specular, m_ShaderType);
				textures.push_back(*defaultTex);
				m_LoadedTex.push_back(*defaultTex);
				m_LoadedTexName.push_back(texPath);
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
		vertices.emplace_back(positions[i], normals[i], Vec3(1.0f, 1.0f, 1.0f), texUVs[i]
		);
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
	}
	return vectors;
}

std::vector<Vec4<float>> ModelLoader::GroupFloatsVec4(const std::vector<float>& floatVec)
{
	std::vector<Vec4<float>> vectors;
	for (int i = 0; i < static_cast<int>(floatVec.size()); )
	{
		vectors.emplace_back(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]);
	}
	return vectors;
}