#pragma once

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

    void Bind(VkWriteDescriptorSet* _vkWriteDescriptorSet, const VkDescriptorSet& _vkDescriptorSet,
    uint32_t _dstBinding, uint32_t _dstArrayElement, uint32_t _descriptorCount, size_t _offset, size_t _size,
    const VkDescriptorBufferInfo& bufferInfo) const;

private:
    void* m_MapMemory = nullptr;
};

END_PCCORE