#ifndef VULKAN_PIPELINE_HELPER
#define VULKAN_PIPELINE_HELPER

#include <vector>

#include <VulkanHeader.h>
#include <spirv_reflect.h>
#include <LowRenderer/RhiPipeline.hpp>

namespace PC_CORE
{
    class ShaderSourceBinary;
} // namespace PC_CORE

namespace Vulkan
{
    class VulkanDescritptorManager;
}

namespace Vulkan::Utils
{
    struct VulkanShaderProgramCreateContext
    {
        std::vector<std::vector<char>> spvModuleSourceCode;
        std::vector<SpvReflectShaderModule> modulesReflected;
        std::vector<vk::ShaderModule> vkShaderModules;
        std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;

        std::vector<vk::PushConstantRange> pushConstantRanges;

        vk::Device device;
    };

    VulkanShaderProgramCreateContext CreateContext(vk::Device _Device, const std::vector<PC_CORE::RhiPipeline::ShaderModuleBinary>& _programShaderCreateInfo);

    void DestroyContext(VulkanShaderProgramCreateContext& _VulkanShaderProgramCreateContext);

    vk::PipelineLayout CreatePipelineLayout(VulkanDescritptorManager& _VulkanDescritptorManager, vk::Device device, VulkanShaderProgramCreateContext& _VulkanShaderProgramCreateContext);

} // namespace Vulkan::Utils

#endif // VULKAN_PIPELINE_HELPER