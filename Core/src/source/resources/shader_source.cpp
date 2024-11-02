#include "resources/shader_source.hpp"

#include <fstream>

#include "rendering/render_harware_interface/RHI.hpp"


using namespace PC_CORE;

ShaderSource::ShaderSource(const fs::path& _path) : Resource(_path)
{
    uint32_t formatIndex = -1;

    if (!Resource::IsFormatValid(ShaderSourceFormat, format, &formatIndex))
    {
        PC_LOGERROR("Shader invalid format")
    }
    format = ShaderSourceFormat[formatIndex];
    shaderType = static_cast<ShaderStageType>(formatIndex);
}

std::vector<char> ShaderSource::GetShaderSourceFile()
{
    if (path.empty())
    {
        PC_LOGERROR("Resource path is empty while trying to get data from it")
        return {};
    }
    
   return ReadFileAsChar(path.generic_string());
}

void ShaderSource::WriteFile(const fs::path& path)
{
    Resource::WriteFile(path);
}



std::vector<char> ShaderSource::ReadFileAsChar(const std::string& _filename)
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
    

    // Create a buffer of the appropriate size + 1 for '\0'
    std::vector<char> buffer(fileSize + 1);

    // Move to the beginning of the file
    file.seekg(0);


    // Read the file data into the buffer
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
    {
        throw std::runtime_error("Failed to read file: " + _filename);
    }
    // Make sure that our data end with end of cahr
    buffer[fileSize] = '\0';

    // Close the file
    file.close();
    return buffer;
}
