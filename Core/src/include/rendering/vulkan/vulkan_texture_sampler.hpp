#pragma once

#include "vulkan_header.h" 

BEGIN_PCCORE

class VulkanTextureSampler
{
public:
    VkSampler textureSampler = VK_NULL_HANDLE;

    static VkDescriptorSetLayoutBinding GetDescriptorSetLayoutBinding(uint32_t binding, uint32_t descriptorCount, VkShaderStageFlags  stageFlags,const VkSampler* pImmutableSamplers = nullptr);
    
    void Init();

    void Destroy();
};

END_PCCORE