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
    
    if (CountainSamplerInfo(_vkSamplerCreateInfo, &idToReturn))
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

void VulkanTextureSampler::Destroy()
{
    for (decltype(m_textureSamplerId)::iterator it  = m_textureSamplerId.begin(); it != m_textureSamplerId.end(); it++)
    {   
        vkDestroySampler(VulkanInterface::GetDevice().device, it->second.textureSampler, nullptr);
    }
}

bool operator==(const VkSamplerCreateInfo& lhs, const VkSamplerCreateInfo& rhs)
{
    return lhs.flags == rhs.flags && lhs.anisotropyEnable == rhs.anisotropyEnable &&
        lhs.borderColor == rhs.borderColor && lhs.compareEnable == rhs.compareEnable && lhs.compareOp == rhs.compareOp
    && lhs.magFilter == rhs.magFilter && lhs.maxAnisotropy == rhs.maxAnisotropy && lhs.maxLod == rhs.maxLod &&
        lhs.minFilter == rhs.minFilter && lhs.minLod == rhs.minLod && lhs.mipmapMode && rhs.mipmapMode
    && lhs.sType == rhs.sType && lhs.unnormalizedCoordinates == rhs.unnormalizedCoordinates &&
        lhs.addressModeU == rhs.addressModeU && lhs.addressModeV == rhs.addressModeV && lhs.addressModeW == rhs.addressModeW &&
            lhs.mipLodBias == rhs.mipLodBias;
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
