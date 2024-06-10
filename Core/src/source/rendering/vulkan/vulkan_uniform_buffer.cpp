#include "rendering/vulkan/vulkan_uniform_buffer.h"

#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_wrapper.hpp"

using namespace PC_CORE;

void VulkanUniformBuffer::Init(void const* _data, size_t _size)
{
    CreateBufferVma(&m_Buffer,&m_Allocation, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU , _size);
    vmaMapMemory(VulkanInterface::GetAllocator(), m_Allocation, &m_MapMemory);
    memcpy(m_MapMemory, _data, _size);
}

void VulkanUniformBuffer::Update(void const* _data, size_t _size)
{
    memcpy(m_MapMemory, _data, _size);
}

void VulkanUniformBuffer::Bind(VkWriteDescriptorSet* _vkWriteDescriptorSet,
    const VkDescriptorSet& _vkDescriptorSet, uint32_t _dstBinding, uint32_t _dstArrayElement, uint32_t _descriptorCount,
    const VkDescriptorBufferInfo& bufferInfo) const
{
 
    _vkWriteDescriptorSet->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    _vkWriteDescriptorSet->dstSet = _vkDescriptorSet;
    _vkWriteDescriptorSet->dstBinding = _dstBinding;
    _vkWriteDescriptorSet->dstArrayElement = _dstArrayElement;
    _vkWriteDescriptorSet->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    _vkWriteDescriptorSet->descriptorCount = _descriptorCount;
    _vkWriteDescriptorSet->pBufferInfo = &bufferInfo;
}

void VulkanUniformBuffer::Destroy()
{
    vmaUnmapMemory(VulkanInterface::GetAllocator(), m_Allocation);
    VulkanBuffer::Destroy();
}

VkDescriptorSetLayoutBinding VulkanUniformBuffer::GetLayoutBinding(uint32_t _binding,
                                                                   uint32_t _descriptorCount, VkShaderStageFlags _stageFlags, const VkSampler* _pImmutableSamplers)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = _binding;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = _descriptorCount;
    uboLayoutBinding.stageFlags = _stageFlags;
    uboLayoutBinding.pImmutableSamplers = _pImmutableSamplers; // Optional

    return uboLayoutBinding;
}
