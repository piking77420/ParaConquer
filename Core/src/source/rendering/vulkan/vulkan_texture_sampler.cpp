#include "rendering/vulkan/vulkan_texture_sampler.hpp"

#include <string>

#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"

using namespace PC_CORE;

VkDescriptorSetLayoutBinding PC_CORE::VulkanTextureSampler::GetDescriptorSetLayoutBinding(uint32_t _binding, uint32_t _descriptorCount, VkShaderStageFlags  _stageFlags,const VkSampler* _pImmutableSamplers)
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = _binding;
    samplerLayoutBinding.descriptorCount = _descriptorCount;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = _pImmutableSamplers;
    samplerLayoutBinding.stageFlags = _stageFlags;

    return samplerLayoutBinding;
}

void VulkanTextureSampler::CreateSampler(const VkSamplerCreateInfo& _vkSamplerCreateInfo, uint32_t* _outId)
{
    // Check if there alraly one
    uint32_t idToReturn = -1;
    
    if (!CountainSamplerInfo(_vkSamplerCreateInfo, &idToReturn))
    {
        VkSampler vkSampler = {};
        const VkResult result = vkCreateSampler(VulkanInterface::GetDevice().device, &_vkSamplerCreateInfo, nullptr, &vkSampler);
        VK_CHECK_ERROR(result, "Failed to Create Sampler");

        m_textureSamplerId.insert({idToReturn,{ _vkSamplerCreateInfo,vkSampler }});
    }
    // if true create and retrun id

    // else return the id 
    *_outId = idToReturn;
}

void VulkanTextureSampler::CreateBasicSampler()
{
    const VkPhysicalDeviceProperties& properties = VulkanInterface::GetPhysicalDevice().devProps;    

    defaultSampler.samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    defaultSampler.samplerInfo.magFilter = VK_FILTER_LINEAR;
    defaultSampler.samplerInfo.minFilter = VK_FILTER_LINEAR;
    defaultSampler.samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    defaultSampler.samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    defaultSampler.samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    defaultSampler.samplerInfo.anisotropyEnable = VK_FALSE;
    defaultSampler.samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    defaultSampler.samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    defaultSampler.samplerInfo.unnormalizedCoordinates = VK_FALSE;
    defaultSampler.samplerInfo.compareEnable = VK_FALSE;
    defaultSampler.samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    defaultSampler.samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    defaultSampler.samplerInfo.mipLodBias = 0.0f;
    defaultSampler.samplerInfo.minLod = 0.0f;
    defaultSampler.samplerInfo.maxLod = 0.0f;

    const VkResult result = vkCreateSampler(VulkanInterface::GetDevice().device, &defaultSampler.samplerInfo, nullptr, &defaultSampler.textureSampler);
}

const VkSampler VulkanTextureSampler::Get(uint32_t _samplerId)
{
    if (!m_textureSamplerId.contains(_samplerId))
        return VK_NULL_HANDLE;

    return m_textureSamplerId.at(_samplerId).textureSampler;
}

void VulkanTextureSampler::Destroy()
{
    for (decltype(m_textureSamplerId)::iterator it  = m_textureSamplerId.begin(); it != m_textureSamplerId.end(); it++)
    {   
        vkDestroySampler(VulkanInterface::GetDevice().device, it->second.textureSampler, nullptr);
    }
    vkDestroySampler(VulkanInterface::GetDevice().device, defaultSampler.textureSampler, nullptr);

}

bool operator==(const VkSamplerCreateInfo& lhs, const VkSamplerCreateInfo& rhs)
{
    return lhs.flags == rhs.flags &&
           lhs.magFilter == rhs.magFilter &&
           lhs.minFilter == rhs.minFilter &&
           lhs.addressModeU == rhs.addressModeU &&
           lhs.addressModeV == rhs.addressModeV &&
           lhs.addressModeW == rhs.addressModeW &&
           lhs.anisotropyEnable == rhs.anisotropyEnable &&
           lhs.compareEnable == rhs.compareEnable &&
           lhs.borderColor == rhs.borderColor &&
           lhs.unnormalizedCoordinates == rhs.unnormalizedCoordinates &&
           lhs.sType == rhs.sType;
}

bool VulkanTextureSampler::CountainSamplerInfo(const VkSamplerCreateInfo& _vkSamplerCreateInfo, uint32_t *_outId)
{

    for (decltype(m_textureSamplerId)::iterator it  = m_textureSamplerId.begin(); it != m_textureSamplerId.end(); it++)
    {   
        if (it->second.samplerInfo == _vkSamplerCreateInfo)
        {
            *_outId = it->first;
            return true;
        }
    }
    *_outId = static_cast<uint32_t>(m_textureSamplerId.size());
    return false;
}
