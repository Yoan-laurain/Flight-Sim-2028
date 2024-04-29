#pragma once

#include <memory>
#include <unordered_map>
#include <string>

enum class ShaderType
{
    NONE = -1,
    BASIC = 0,
    HEIGHTMAP = 1,
    SKYBOX = 2,
    EROSION = 3,
    TERRAIN  = 4
};

class Shader;

class ShaderManager
{
    public:

        ShaderManager();
        ~ShaderManager() = default;
    
        void RunComputeShader(const Shader* shader, int numIterations = 0, float wGroupSize = 64) const;

        template <typename T>
        Shader* CreateShader(ShaderType shaderType, const std::string& filepath)
        {
            T* shader = new T(filepath);
            shader->Bind();
			RegisterShader(shaderType, shader);
			return shader;
        }

        Shader* GetShader(ShaderType shaderType);

    private:
        std::unordered_map<ShaderType, std::unique_ptr<Shader>> m_Shaders;
    
        void RegisterShader(ShaderType shaderType, Shader* shader);
};