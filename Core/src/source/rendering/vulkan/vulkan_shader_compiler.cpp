#include "rendering/vulkan/vulkan_shader_compiler.hpp"

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

void VulkanShaderCompiler::CompileToSpv(const std::filesystem::path& _shaderSourcePath)
{
    if (vulkanEnvironementPath.empty())
    {
        PC_LOGERROR("vulkanEnvironementPath is empty while tryng compiling shader")
        exit(1);
    }

    //"C:/VulkanSDK/x.x.x.x/Bin32/glslc.exe shader.vert -o vert.spv"
    system((vulkanEnvironementPath + "Bin32/glslc.exe " + _shaderSourcePath.generic_string() + " -o " + _shaderSourcePath.filename().generic_string()).c_str());
}

uint8_t* VulkanShaderCompiler::GetData()
{
    return m_Data;
}
