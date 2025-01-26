#include "resources/shader_source.hpp"

#include <fstream>
#include <iostream>

#include <regex>
#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

using namespace PC_CORE;


glslang_stage_t GetGlangShaderStage(ShaderStageType _shaderType)
{
    switch (_shaderType)
    {
    case ShaderStageType::VERTEX:
        return GLSLANG_STAGE_VERTEX;
    case ShaderStageType::TESSCONTROL:
        return GLSLANG_STAGE_TESSCONTROL;
    case ShaderStageType::TESSEVALUATION:
        return GLSLANG_STAGE_TESSEVALUATION;
    case ShaderStageType::GEOMETRY:
        return GLSLANG_STAGE_GEOMETRY;
    case ShaderStageType::FRAGMENT:
        return GLSLANG_STAGE_FRAGMENT;
    case ShaderStageType::COMPUTE:
        return GLSLANG_STAGE_COMPUTE;
    case ShaderStageType::RAYGEN:
        return GLSLANG_STAGE_RAYGEN_NV;
    case ShaderStageType::INTERSECT:
        return GLSLANG_STAGE_INTERSECT;
    case ShaderStageType::ANYHIT:
        return GLSLANG_STAGE_ANYHIT;
    case ShaderStageType::CLOSESTHIT:
        return GLSLANG_STAGE_CLOSESTHIT;
    case ShaderStageType::MISS:
        return GLSLANG_STAGE_MISS;
    case ShaderStageType::CALLABLE:
        return GLSLANG_STAGE_CALLABLE;
    case ShaderStageType::TASK:
        return GLSLANG_STAGE_TASK;
    case ShaderStageType::MESH:
        return GLSLANG_STAGE_MESH;
    case ShaderStageType::COUNT:
    default:
        throw std::invalid_argument("Invalid shader stage type");
    }
}


ShaderSource::ShaderSource(const fs::path& _path) : Resource(_path)
{
    uint32_t formatIndex = -1;

    if (!Resource::IsFormatValid(ShaderSourceFormat, format, &formatIndex))
    {
        PC_LOGERROR("Shader invalid format")
    }
    
    format = ShaderSourceFormat[formatIndex];
    m_ShaderType = static_cast<ShaderStageType>(formatIndex);
}

std::vector<char> ShaderSource::GetShaderSourceFile()
{
    if (path.empty())
    {
        PC_LOGERROR("Resource path is empty while trying to get data from it")
        return {};
    }

    std::vector<char> source = Resource::ReadFile(path.generic_string());
    std::vector<char> sWithInclude = IncludePath(source, path);    
    sWithInclude.push_back('\0');


    return sWithInclude;
}


bool ShaderSource::GetAsSpriv(std::vector<char>* _buffer)
{
    std::vector<char> sourceCode = GetShaderSourceFile();
   
    auto resource = glslang_default_resource();


    const glslang_input_t input =
    {
        .language = GLSLANG_SOURCE_GLSL,
        .stage = GetGlangShaderStage(m_ShaderType),
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_3,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_6,
        .code = sourceCode.data(),
        .default_version = 100,
        .default_profile = GLSLANG_NO_PROFILE,
        .force_default_version_and_profile = false,
        .forward_compatible = false,
        .messages = GLSLANG_MSG_DEFAULT_BIT,
        .resource = resource
    };

    glslang_shader_t* shader = glslang_shader_create(&input);


    if (!glslang_shader_preprocess(shader, &input))
    {
        std::string logInfo = glslang_shader_get_info_log(shader);
        std::string logdebug = glslang_shader_get_info_debug_log(shader);
        
        PC_LOGERROR("Error While preprocess Shader")

        // use glslang_shader_get_info_log() and glslang_shader_get_info_debug_log()
    }

    if (!glslang_shader_parse(shader, &input))
    {
        std::string logInfo = glslang_shader_get_info_log(shader);
        std::string logdebug = glslang_shader_get_info_debug_log(shader);
            PC_LOGERROR("Error While glslang_shader_parse Shader")

        // use glslang_shader_get_info_log() and glslang_shader_get_info_debug_log()
    }

    
    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
    {
        std::string logInfo = glslang_shader_get_info_log(shader);
        std::string logdebug = glslang_shader_get_info_debug_log(shader);
        PC_LOGERROR("Error While glslang_program_link Shader")
        // use glslang_program_get_info_log() and glslang_program_get_info_debug_log();
    }

    glslang_program_SPIRV_generate(program, input.stage);

    if (glslang_program_SPIRV_get_messages(program))
    {
        printf("%s", glslang_program_SPIRV_get_messages(program));
    }

    const size_t spvCodeSize = glslang_program_SPIRV_get_size(program) * sizeof(uint32_t);
    const uint32_t* spvPointer = glslang_program_SPIRV_get_ptr(program);

    *_buffer = std::vector<char>(reinterpret_cast<const char*>(spvPointer),
        reinterpret_cast<const char*>(spvPointer) + spvCodeSize);

    glslang_shader_delete(shader);
    glslang_program_delete(program);


    return true;
}





void ShaderSource::WriteFile(const fs::path& folder)
{
    Resource::WriteFile(folder);
}

void ShaderSource::CompileToSpriv()
{
    std::string filePath = SHADER_CACHE_PATH + name + "_spv" + format;
    std::fstream f(filePath, std::ios::binary | std::ios::out);
    std::vector<char> sourceSpriv;

    if (!GetAsSpriv(&sourceSpriv))
    {
        PC_LOGERROR("Failed to read shader source file for writing shader spriv cache");
        return;
    }
    
    if (!f.is_open())
    {
        std::cerr << "Failed to open file " << filePath << std::endl;
        PC_LOGERROR("File is not open")
        return;
    }
    
    f << sourceSpriv.data();
    f.close();
}

ShaderSource::~ShaderSource()
{
    
}


std::vector<char> ShaderSource::IncludePath(const std::vector<char>& source, const std::filesystem::path& path)
{
    std::regex includeRegex(R"(#\s*include\s*\"([^\"]+)\")");
    std::string sourceStr(source.begin(), source.end());

    if (!std::regex_search(sourceStr, includeRegex))
    {
        return source; // Return the original source if no #include is found
    }

    std::string sourceWithoutInclude;
    size_t globalIndexInSourceShader = 0;

    auto matchesBegin = std::sregex_iterator(sourceStr.begin(), sourceStr.end(), includeRegex);
    auto matchesEnd = std::sregex_iterator();

    for (auto it = matchesBegin; it != matchesEnd; ++it)
    {
        const std::smatch& match = *it;
        size_t matchStart = match.position();
        size_t matchEnd = matchStart + match.length();

        // Add the part of the source before the #include
        sourceWithoutInclude.
            append(sourceStr.substr(globalIndexInSourceShader, matchStart - globalIndexInSourceShader));

        // Extract the include file path from the match group
        std::string includeFile = match.str(1);

        // Resolve the include path relative to the parent path
        std::filesystem::path includeFilePath = path.parent_path() / includeFile;

        // Read the included file
        std::vector<char> includedSource = Resource::ReadFile(includeFilePath.generic_string());

        // Recursively process the included file for nested includes
        includedSource = IncludePath(includedSource, includeFilePath);

        // Append the processed include file content
        sourceWithoutInclude.append(includedSource.begin(), includedSource.end());

        // Update the global index
        globalIndexInSourceShader = matchEnd;
    }

    // Append the remaining part of the source after the last #include
    sourceWithoutInclude.append(sourceStr.substr(globalIndexInSourceShader));

    // Convert the result back to std::vector<char>
    return std::vector(sourceWithoutInclude.begin(), sourceWithoutInclude.end());
}

