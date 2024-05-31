#include "resources/shader_source.hpp"

#include "log.hpp"
#include "rendering/vulkan/vulkan_shader_compiler.hpp"

using namespace PC_CORE;

void ShaderSource::Load(const fs::path& path)
{
    uint32_t formatIndex = -1;
    const std::string currentFormat = path.filename().extension().generic_string();
    
    if (!IResource::IsFormatValid(ShaderSourceFormat, currentFormat, &formatIndex))
    {
        PC_LOGERROR("Shader invalid format")
    }

    name = path.filename().generic_string();
    format = ShaderSourceFormat.at(formatIndex);
    resourcePath = path;
    shaderType = static_cast<ShaderType>(formatIndex);

    if (!VulkanShaderCompiler::CompileToSpv(resourcePath, format, &data))
    {
        PC_LOGERROR("Failed to CompileToSpv")
    }
}
