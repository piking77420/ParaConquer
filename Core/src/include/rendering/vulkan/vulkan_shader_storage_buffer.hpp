#pragma once

#include "vulkan_buffer.hpp"

BEGIN_PCCORE

class VulkanShaderStorageBuffer : public PC_CORE::VulkanBuffer
{
public:
    static VkDescriptorSetLayoutBinding GetLayoutBinding(uint32_t _binding, uint32_t _descriptorCount, VkShaderStageFlags  _stageFlags,const VkSampler* _pImmutableSamplers = nullptr);
    
    void Init(const size_t _size);

    void Update(void const* _data, size_t _size);

    void Destroy() override;

    void Bind(VkWriteDescriptorSet* _vkWriteDescriptorSet, const VkDescriptorSet& _vkDescriptorSet,
    uint32_t _dstBinding, uint32_t _dstArrayElement, uint32_t _descriptorCount,
    const VkDescriptorBufferInfo& bufferInfo) const;

private:
    void* m_MapMemory = nullptr;

};

END_PCCORE