#include "resources/shader_source.hpp"

#include <fstream>

#include <regex>


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

std::string ShaderSource::GetShaderSourceFile()
{
    if (path.empty())
    {
        PC_LOGERROR("Resource path is empty while trying to get data from it")
        return {};
    }

    std::vector<char> source = ReadFileAsChar(path.generic_string());
    std::string sWithInclude = IncludePath(source.data(), path);
    
   return sWithInclude;
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

std::string ShaderSource::IncludePath(const std::string& source, const std::filesystem::path& path)
{
    std::regex regex("#include");

    if (!std::regex_search(source, regex))
    {
        return source;
    }

    // TO DO make include just replace at their inplacement
    auto words_begin =
        std::sregex_iterator(source.begin(), source.end(), regex);
    auto words_end = std::sregex_iterator();

    const size_t nbr = std::distance(words_begin, words_end);

    std::string sourceWithoutInclude;

    size_t globalIndexInSourceShader = 0;

    for (std::sregex_iterator i = words_begin; i != words_end; ++i)
    {

        std::smatch match = *i;
        std::string matchedWord = match.str();
        std::size_t position = match.position();
        std::size_t endPosition = match.position() + matchedWord.size();

        // Append all the file from the start to the include start
        sourceWithoutInclude.append(source.substr(globalIndexInSourceShader, position - globalIndexInSourceShader));


        // Get the #include "..."
        char c = 0;
        std::size_t startIndexLocalInclude = 0;

        while (c != '"' && (endPosition + startIndexLocalInclude) < source.size())
        {
            c = source.at(endPosition + startIndexLocalInclude);
            startIndexLocalInclude++;
        }

        std::size_t includeStart = endPosition + startIndexLocalInclude;
        startIndexLocalInclude++;

        std::size_t endIndexLocalInclude = 0;
        c = 0;

        while (c != '"' && (includeStart + endIndexLocalInclude) < source.size())
        {
            c = source.at(includeStart + endIndexLocalInclude);
            endIndexLocalInclude++;
        }

        // Get the Include Path
        const std::string includePaths = source.substr(includeStart, endIndexLocalInclude - 1);
        const std::string parentPathAsString = path.parent_path().generic_string() + '/';
        std::filesystem::path includePathPath = std::filesystem::path(parentPathAsString + includePaths);

        // Get the include file as string
        std::string sourceFromIncludeMacro = GetFileAsString(includePathPath);
        sourceFromIncludeMacro = IncludePath(sourceFromIncludeMacro, includePathPath);

        // REMOVE THE \0 character DONT'TOUCH very important
        sourceFromIncludeMacro.pop_back();


        sourceWithoutInclude.append(sourceFromIncludeMacro);


        globalIndexInSourceShader = includeStart + endIndexLocalInclude;
    }

    // GOBING ALL THE ENTIRE REST
    const size_t count = source.size() - globalIndexInSourceShader;
    const std::string remining = source.substr(globalIndexInSourceShader, count);
    sourceWithoutInclude += remining;

    return sourceWithoutInclude;
}

PC_CORE_API std::string ShaderSource::GetFileAsString(const std::filesystem::path& path)
{
    std::string fileName = path.generic_string();

    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + fileName);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());

    std::string buffer;
    buffer.resize(fileSize);

    file.seekg(0);

    if (!file.read(&buffer[0], fileSize))
    {
        throw std::runtime_error("Failed to read file: " + fileName);
    }

    file.close();

    buffer.push_back('\0');

    file.close();
    return buffer;
}
