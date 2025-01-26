#include "resources/vulkan_shader_program.hpp"

#include "rhi_vulkan_parser.hpp"
#include "vulkan_context.hpp"
#include "vulkan_header.h"

using namespace Vulkan;

VulkanShaderProgram::VulkanShaderProgram(const PC_CORE::ProgramShaderCreateInfo& _programShaderCreateInfo) : ShaderProgram(_programShaderCreateInfo)
{
    std::vector<std::vector<char>> spvModuleSourceCode = std::vector<std::vector<char>>(m_ProgramShaderCreateInfo.shaderInfo.shaderSources.size());

    for (size_t i = 0; i < spvModuleSourceCode.size(); i++)
    {
        const std::pair<PC_CORE::ShaderStageType, std::string>& shaderSource = _programShaderCreateInfo.shaderInfo.shaderSources[i];
        const char* format = nullptr;
         if (!Resource::GetFormatFromValue(PC_CORE::ShaderSourceFormat, shaderSource.first, &format))
         {
             PC_LOGERROR("Failed to parse shader source for shader ");
         }
        std::string spvFile = SHADER_CACHE_PATH + shaderSource.second;

        spvModuleSourceCode[i] = ReadFile(spvFile);
    }

    std::vector<vk::ShaderModule> vkShaderModules(spvModuleSourceCode.size());
    std::vector<vk::PipelineShaderStageCreateInfo> PipelineShaderStageCreateInfos(vkShaderModules.size());
    
    std::shared_ptr<VulkanDevice> device = std::reinterpret_pointer_cast<VulkanDevice>( VulkanContext::GetContext().rhiDevice);

    // Create Modules
    for (size_t i = 0; i < spvModuleSourceCode.size(); i++)
    {
        vk::ShaderModuleCreateInfo createInfo = {};
        createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
        createInfo.codeSize = spvModuleSourceCode[i].size();
        createInfo.pCode = reinterpret_cast<uint32_t*>(spvModuleSourceCode[i].data());
        
        VK_CALL(device->GetDevice().createShaderModule(&createInfo, nullptr, &vkShaderModules[i]));
    }

    // CreatePipelineShaderStage 

    for (size_t i = 0; i < PipelineShaderStageCreateInfos.size(); i++)
    {
        vk::PipelineShaderStageCreateInfo createInfo = {};
        createInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        createInfo.stage = RhiToShaderStage(_programShaderCreateInfo.shaderInfo.shaderSources[i].first);
        createInfo.module = vkShaderModules[i];
        createInfo.pName = "main";
    }
    
    constexpr std::array<vk::DynamicState,9> dynamicStateArray =
    {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
        vk::DynamicState::eLineWidth,
        vk::DynamicState::eDepthBias,
        vk::DynamicState::eBlendConstants,
        vk::DynamicState::eDepthBounds,
        vk::DynamicState::eStencilCompareMask,
        vk::DynamicState::eStencilWriteMask,
        vk::DynamicState::eStencilReference
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateArray.size());
    dynamicState.pDynamicStates = dynamicStateArray.data();
    
    for (size_t i = 0; i < spvModuleSourceCode.size(); i++)
    {
        device->GetDevice().destroyShaderModule(vkShaderModules[i]);
    }
}
