#include "resources/shader_source.hpp"

#include <fstream>


using namespace PC_CORE;

void ShaderSource::SetPath(const fs::path& _path)
{
    path = _path;
    
    uint32_t formatIndex = -1;
    const std::string currentFormat = path.filename().extension().generic_string();
    
    if (!IResource::IsFormatValid(ShaderSourceFormat, currentFormat, &formatIndex))
    {
        PC_LOGERROR("Shader invalid format")
    }

    name = path.filename().generic_string();
    format = ShaderSourceFormat[formatIndex];
    shaderType = static_cast<LowLevelShaderStageType>(formatIndex);
}

std::vector<uint8_t> ShaderSource::GetData()
{
    if (path.empty())
    {
        PC_LOGERROR("Resource path is empty while trying to get data from it")
        return {};
    }
    
    ReadFile(path.generic_string());
}

void ShaderSource::WriteFile(const fs::path& path)
{
    IResource::WriteFile(path);
}

ShaderSource::~ShaderSource()
{
    
}

std::vector<uint8_t> ShaderSource::ReadFile(const std::string& _filename)
{
    
    // Open file in binary mode at the end of the file to get the file size easily
    std::ifstream file(_filename, std::ios::ate | std::ios::binary);

    // Check if the file was opened successfully
    if (!file.is_open()) 
    {
        throw std::runtime_error("Failed to open file: " + _filename);
    }

    // Get the size of the file
    size_t fileSize = static_cast<size_t>(file.tellg());
    
    // Create a buffer of the appropriate size
    std::vector<uint8_t> buffer(fileSize);

    // Move to the beginning of the file
    file.seekg(0);

    // Read the file data into the buffer
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
    {
        throw std::runtime_error("Failed to read file: " + _filename);
    }

    // Close the file
    file.close();
    return buffer;
}
