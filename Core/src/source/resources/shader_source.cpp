#include "resources/shader_source.hpp"

#include <fstream>

#include "log.hpp"
#include "rendering/RHI.hpp"

using namespace PC_CORE;

void ShaderSource::SetPath(const fs::path& path)
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
}

std::vector<uint8_t> ShaderSource::GetData()
{
    if (resourcePath.empty())
    {
        PC_LOGERROR("Resource path is empty while trying to get data from it")
        return {};
    }
    
    ReadFile(resourcePath.generic_string());
}

void ShaderSource::WriteFile(const fs::path& path)
{
    IResource::WriteFile(path);
}

ShaderSource::~ShaderSource()
{
    
}

std::vector<char> ShaderSource::ReadFile(const std::string& _filename)
{
    std::ifstream file(_filename, std::ios::ate | std::ios::binary);
    
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
