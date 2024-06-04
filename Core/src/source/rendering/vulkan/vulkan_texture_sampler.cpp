#include "rendering/vulkan/vulkan_texture_sampler.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

VkDescriptorSetLayoutBinding PC_CORE::VulkanTextureSampler::GetDescriptorSetLayoutBinding(uint32_t binding, uint32_t descriptorCount, VkShaderStageFlags  stageFlags,const VkSampler* pImmutableSamplers)
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = binding;
    samplerLayoutBinding.descriptorCount = descriptorCount;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = pImmutableSamplers;
    samplerLayoutBinding.stageFlags = stageFlags;

    return samplerLayoutBinding;
}

void PC_CORE::VulkanTextureSampler::Init()
{
    const VkPhysicalDeviceProperties& properties = VulkanInterface::GetPhysicalDevice().devProps;

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VkResult result = vkCreateSampler(VulkanInterface::GetDevice().device, &samplerInfo, nullptr, &textureSampler);
    
    VK_CHECK_ERROR(result, "Failed to Create Sampler");
}

void PC_CORE::VulkanTextureSampler::Destroy()
{
    vkDestroySampler(VulkanInterface::GetDevice().device, textureSampler, nullptr);
}
