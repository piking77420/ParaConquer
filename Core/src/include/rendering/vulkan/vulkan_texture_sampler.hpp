#pragma once

#include <map>

#include "vulkan_header.h" 

BEGIN_PCCORE

class VulkanTextureSampler
{
public:
    static VkDescriptorSetLayoutBinding GetDescriptorSetLayoutBinding(uint32_t _binding, uint32_t _descriptorCount, VkShaderStageFlags  _stageFlags,
        const VkSampler* pImmutableSamplers = nullptr);

    
    struct Sampler
    {
        VkSamplerCreateInfo samplerInfo;
        VkSampler textureSampler = VK_NULL_HANDLE;
    };
    
    void CreateSampler(const VkSamplerCreateInfo& _vkSamplerCreateInfo, uint32_t* _outId);

    const VkSampler Get(uint32_t _samplerId);
    
    void Destroy();
    
private:
    std::map<uint32_t,Sampler> m_textureSamplerId;

    bool CountainSamplerInfo(const VkSamplerCreateInfo& _vkSamplerCreateInfo, uint32_t *_outId);
};

END_PCCORE
