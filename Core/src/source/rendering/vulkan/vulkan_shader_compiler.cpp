#include "rendering/vulkan/vulkan_shader_compiler.hpp"

#include <fstream>

#include "log.hpp"

using namespace PC_CORE;

void VulkanShaderCompiler::GetEnvironementVariable()
{

    
    char* envVulkanName = nullptr;
    size_t bufferCount = -1;
    _dupenv_s(&envVulkanName, &bufferCount,"VULKAN_SDK");

    if (bufferCount == -1)
    {
        PC_LOGERROR("vulkanEnvironmentPath was not found")
        exit(1);
    }

    vulkanEnvironmentPath.resize(bufferCount);

    for (size_t i = 0; i < bufferCount; i++)
    {
        vulkanEnvironmentPath[i] = envVulkanName[i];
    }
}



bool VulkanShaderCompiler::CompileToSpv(const std::filesystem::path& _shaderSourcePath, const std::string& _extension,std::vector<char>* _data)
{
    if (vulkanEnvironmentPath.empty())
    {
        PC_LOGERROR("vulkanEnvironmentPath is empty while tryng compiling shader")

        throw std::runtime_error("vulkanEnvironmentPath is empty while tryng compiling shader");
    }

    /// TO DO SHADER C
    //"C:/VulkanSDK/x.x.x.x/Bin32/glslc.exe shader.vert -o vert.spv"
    std::string shaderName = _shaderSourcePath.filename().generic_string();
    std::string shaderSpvPath = std::string(vulkanShaderCache) + "/" +  shaderName + ".spv";

    std::string command = std::string(vulkanEnvironmentPath).c_str();  // NOLINT(readability-redundant-string-cstr)
    command +=  std::string("/Bin/glslc.exe");
    std::string file = _shaderSourcePath.filename().generic_string();
    std::filesystem::path p = std::filesystem::current_path().parent_path();
    const std::string commandToSend = command + " " + _shaderSourcePath.generic_string() + " -o " + shaderSpvPath;
    
    system(commandToSend.data());
    
    ReadFile(shaderSpvPath, _data);

    return true;
}

void VulkanShaderCompiler::ReadFile(const std::string& _filename, std::vector<char>* data)
{
    std::ifstream file(_filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    const size_t fileSize = (size_t) file.tellg();
    data->resize(fileSize);

    file.seekg(0);
    file.read(data->data(), fileSize);

    file.close();
}