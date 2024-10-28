#include "front_end/vulkan_shader_compiler.hpp"

#include <fstream>
#include <regex>
#include <vector>
#include <glslang/Include/glslang_c_shader_types.h>

#include "front_end/vulkan_harware_wrapper.hpp"

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

#pragma region LowLevelShaderStageToShaderrcKind


glslang_stage_t GetShaderStageToGlslangStage(PC_CORE::ShaderStageType _lowLevelShaderStage)
{
    switch (_lowLevelShaderStage)
    {
    case PC_CORE::ShaderStageType::VERTEX:
        return GLSLANG_STAGE_VERTEX;
        break;
    case PC_CORE::ShaderStageType::FRAGMENT:
        return GLSLANG_STAGE_FRAGMENT;
        break;
    case PC_CORE::ShaderStageType::GEOMETRY:
        return GLSLANG_STAGE_GEOMETRY;
        break;
    case PC_CORE::ShaderStageType::TESSELATION:
        return GLSLANG_STAGE_TESSCONTROL;
        break;
    case PC_CORE::ShaderStageType::COMPUTE:
        return GLSLANG_STAGE_COMPUTE;
        break;
    default: ;
    }

    return static_cast<glslang_stage_t>(std::numeric_limits<uint32_t>::max());
}


#pragma endregion LowLevelShaderStageToShaderrcKind


Vulkan::VulkanShaderCompiler::VulkanShaderCompiler()
{
    glslang_initialize_process();
}

Vulkan::VulkanShaderCompiler::~VulkanShaderCompiler()
{
    glslang_finalize_process();
}

void Vulkan::VulkanShaderCompiler::CreateModuleFromSource(vk::Device _device, const char* _source, const char* _path,
                                                         PC_CORE::ShaderStageType _lowLevelShaderStage,SpvReflectShaderModule* _ReflectedModule,
                                                         vk::ShaderModule* _shaderModule)
{
    std::filesystem::path path = _path;
    std::string source = IncludePath(_source, path);
    
    auto resource = glslang_default_resource();
    const glslang_input_t input =
    {
        .language = GLSLANG_SOURCE_GLSL,
        .stage = GetShaderStageToGlslangStage(_lowLevelShaderStage),
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_3,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_3,
        .code = source.c_str(),
        .default_version = 100,
        .default_profile = GLSLANG_NO_PROFILE,
        .force_default_version_and_profile = false,
        .forward_compatible = false,
        .messages = GLSLANG_MSG_DEFAULT_BIT,
        .resource = resource,
    };
    
    glslang_shader_t* shader = glslang_shader_create(&input);
    
    if (!glslang_shader_preprocess(shader, &input))
    {
        // use glslang_shader_get_info_log() and glslang_shader_get_info_debug_log()
        const char* preprocessInfoLog = glslang_shader_get_info_log(shader);
        const char* preprocessInfDebugoLog = glslang_shader_get_info_debug_log(shader);
        std::string logPreprocess = preprocessInfoLog;
        logPreprocess += '\n';
        logPreprocess += preprocessInfDebugoLog;
        std::cout << logPreprocess << '\n';
    }

    if (!glslang_shader_parse(shader, &input))
    {
         //use glslang_shader_get_info_log() and glslang_shader_get_info_debug_log()
        const char* parseInfoLog = glslang_shader_get_info_log(shader);
        const char* parseInfDebugoLog = glslang_shader_get_info_debug_log(shader);
        std::string logparse = parseInfoLog;
        logparse += '\n';
        logparse += parseInfDebugoLog;
        std::cout << logparse << '\n';
    }


    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
    {
        // use glslang_program_get_info_log() and glslang_program_get_info_debug_log();
    }

    glslang_program_SPIRV_generate(program, input.stage);

    if (glslang_program_SPIRV_get_messages(program))
    {
        printf("%s", glslang_program_SPIRV_get_messages(program));
    }

    const size_t spvCodeSize = glslang_program_SPIRV_get_size(program) * sizeof(unsigned int);
    const uint32_t* spvPointer = glslang_program_SPIRV_get_ptr(program);

    vk::ShaderModuleCreateInfo moduleCreateInfo = {};
    moduleCreateInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
    moduleCreateInfo.pCode = spvPointer;
    moduleCreateInfo.codeSize = spvCodeSize;

    spvReflectCreateShaderModule(spvCodeSize, spvPointer, _ReflectedModule);

    
    *_shaderModule = _device.createShaderModule(moduleCreateInfo, nullptr);

    glslang_shader_delete(shader);
    glslang_program_delete(program);
}

std::string Vulkan::VulkanShaderCompiler::IncludePath(const std::string& source, const std::filesystem::path& path)
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
    
    size_t nbr = std::distance(words_begin, words_end);

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

std::string Vulkan::VulkanShaderCompiler::GetFileAsString(const std::filesystem::path& path)
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
