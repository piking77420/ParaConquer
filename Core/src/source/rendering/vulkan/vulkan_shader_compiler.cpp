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
        PC_LOGERROR("vulkanEnvironementPath was not found")
        exit(1);
    }

    vulkanEnvironementPath.resize(bufferCount);

    for (size_t i = 0; i < bufferCount; i++)
    {
        vulkanEnvironementPath[i] = envVulkanName[i];
    }
}



bool VulkanShaderCompiler::CompileToSpv(const std::filesystem::path& _shaderSourcePath, const std::string& _extension,std::vector<char>* _data)
{
    if (vulkanEnvironementPath.empty())
    {
        PC_LOGERROR("vulkanEnvironementPath is empty while tryng compiling shader")
        exit(1);
        return false;
    }
    
    std::string command = "C:\VulkanSDK\1.3.283.0";
    std::string add = std::string("Bin32/glslc.exe " + _shaderSourcePath.generic_string() + " -o " + _shaderSourcePath.filename().generic_string());

    for (size_t i = 0; i < add.size(); ++i)
    {
        command.push_back(add[i]);
    }
    
    //"C:/VulkanSDK/x.x.x.x/Bin32/glslc.exe shader.vert -o vert.spv"
    if (!system((command).c_str()))
        return false;

    // Load shader code
    std::string spvPath = _shaderSourcePath.parent_path().generic_string() + '/' + _shaderSourcePath.filename().stem().generic_string() + '_' + _extension + ".spv"; 
    
    ReadFile(spvPath,_data);
    
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