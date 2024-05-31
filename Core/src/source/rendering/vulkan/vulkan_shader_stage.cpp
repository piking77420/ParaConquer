#include "rendering/vulkan/vulkan_shader_stage.hpp"

#include <set>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

void VulkanShaderStage::Init(const std::vector<const ShaderSource*>& _shaderSource)
{
    m_ShaderModules.clear();
    const VkDevice& device = VulkanInterface::GetDevice().device;

    // Check if shader type are unique in this pipeline no 2 vertex shader
    std::set<ShaderSource::ShaderType> shaderTypes;
    for (const ShaderSource* shader : _shaderSource)
    {
        if (!shaderTypes.emplace(shader->shaderType).second)
        {
            ShaderSource::ShaderType type = shader->shaderType;
            throw std::runtime_error("Duplicate shader type in pipeline: " + std::to_string(static_cast<int>(type)));
        }
    }
    
    for (const ShaderSource* shader : _shaderSource)
    {
        const std::vector<char>& shaderData = shader->data;

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderData.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderData.data());

        VkShaderModule shaderModule = VK_NULL_HANDLE;

        VkResult r = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
        VK_CHECK_ERROR(r, "failed to create shader module from " + shader->name)

        m_ShaderModules.push_back({shader, shaderModule});
    }

    Compile(device);
}

void VulkanShaderStage::Compile(const VkDevice& _device)
{

    for (const ShaderWithModule& shader : m_ShaderModules)
    {
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = ShaderTypeTOVukan(shader.shaderSource->shaderType);
        shaderStageInfo.module = shader.vkShaderModule;
        shaderStageInfo.pName = "main";
        vkPipelineShaderStageCreateInfo.push_back(shaderStageInfo);
    }
    
}

VkShaderStageFlagBits VulkanShaderStage::ShaderTypeTOVukan(ShaderSource::ShaderType shaderType)
{
    switch (shaderType)
    {
    case ShaderSource::ShaderType::VERTEX:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case ShaderSource::ShaderType::FRAGMENT:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    default:
        PC_LOGERROR("Unsuported Shader Type");
    }

    return VK_SHADER_STAGE_VERTEX_BIT;
}

void VulkanShaderStage::Destroy(const VkDevice& _device)
{
    for (const ShaderWithModule& shader : m_ShaderModules)
    {
        vkDestroyShaderModule(_device, shader.vkShaderModule, nullptr);
    }

    vkPipelineShaderStageCreateInfo.clear();
}
