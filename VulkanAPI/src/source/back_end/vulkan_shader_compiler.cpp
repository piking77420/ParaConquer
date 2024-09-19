#include "back_end/vulkan_shader_compiler.hpp"


#include <glslang/Include/glslang_c_shader_types.h>

#include "back_end/vulkan_harware_wrapper.hpp"

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

#pragma region LowLevelShaderStageToShaderrcKind


glslang_stage_t GetShaderStageToGlslangStage(PC_CORE::LowLevelShaderStageType _lowLevelShaderStage)
{
    switch (_lowLevelShaderStage)
    {
    case PC_CORE::LowLevelShaderStageType::VERTEX:
        return GLSLANG_STAGE_VERTEX;
        break;
    case PC_CORE::LowLevelShaderStageType::FRAGMENT:
        return GLSLANG_STAGE_FRAGMENT;
        break;
    case PC_CORE::LowLevelShaderStageType::GEOMETRY:
        return GLSLANG_STAGE_GEOMETRY;
        break;
    case PC_CORE::LowLevelShaderStageType::TESSELATION:
        return GLSLANG_STAGE_TESSCONTROL;
        break;
    case PC_CORE::LowLevelShaderStageType::COMPUTE:
        return GLSLANG_STAGE_COMPUTE;
        break;
    default: ;
    }

    return static_cast<glslang_stage_t>(std::numeric_limits<uint32_t>::max());
}


#pragma endregion LowLevelShaderStageToShaderrcKind


VK_NP::VulkanShaderCompiler::VulkanShaderCompiler()
{
    glslang_initialize_process();
    m_Device = VulkanContext::currentContext->device;
}

VK_NP::VulkanShaderCompiler::~VulkanShaderCompiler()
{
    glslang_finalize_process();
}

void VK_NP::VulkanShaderCompiler::CreateModuleFromSource(const char* _source,
                                                         PC_CORE::LowLevelShaderStageType _lowLevelShaderStage,SpvReflectShaderModule* _ReflectedModule,
                                                         vk::ShaderModule* _shaderModule)
{


    auto resource = glslang_default_resource();
    const glslang_input_t input =
    {
        .language = GLSLANG_SOURCE_GLSL,
        .stage = GetShaderStageToGlslangStage(_lowLevelShaderStage),
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_1,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_3,
        .code = _source,
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

    
    *_shaderModule = m_Device.createShaderModule(moduleCreateInfo, nullptr);

    glslang_shader_delete(shader);
    glslang_program_delete(program);
}
