﻿#pragma once

#include "vulkan_buffer.hpp"
#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanUniformBuffer : public VulkanBuffer
{
public:
    void Destroy() override;

    static VkDescriptorSetLayoutBinding GetLayoutBinding(uint32_t binding, uint32_t descriptorCount, VkShaderStageFlags  stageFlags,const VkSampler* pImmutableSamplers = nullptr);
    
    void Init(void const* _data, size_t _size);

    void Update(void const* _data, size_t _size);


private:
    void* m_MapMemory = nullptr;
};

END_PCCORE