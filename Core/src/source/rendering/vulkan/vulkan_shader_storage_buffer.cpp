#include "rendering/vulkan/vulkan_shader_storage_buffer.hpp"

#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_wrapper.hpp"

using namespace PC_CORE;

VkDescriptorSetLayoutBinding VulkanShaderStorageBuffer::GetLayoutBinding(uint32_t _binding,
    uint32_t _descriptorCount, VkShaderStageFlags _stageFlags, const VkSampler* _pImmutableSamplers)
{
    VkDescriptorSetLayoutBinding shaderBufferLayoutBinding{};
    shaderBufferLayoutBinding.binding = _binding;
    shaderBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    shaderBufferLayoutBinding.descriptorCount = _descriptorCount;
    shaderBufferLayoutBinding.stageFlags = _stageFlags;
    shaderBufferLayoutBinding.pImmutableSamplers = _pImmutableSamplers; // Optional

    return shaderBufferLayoutBinding;
}

void VulkanShaderStorageBuffer::Init(const size_t _size)
{
    CreateBufferVma(&m_Buffer,&m_Allocation, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU , _size);
    vmaMapMemory(VulkanInterface::GetAllocator(), m_Allocation, &m_MapMemory);
}

void VulkanShaderStorageBuffer::Update(void const* _data, size_t _size)
{
    memcpy(m_MapMemory, _data, _size);
}

void VulkanShaderStorageBuffer::Destroy()
{
    vmaUnmapMemory(VulkanInterface::GetAllocator(), m_Allocation);
    VulkanBuffer::Destroy();
}

void VulkanShaderStorageBuffer::Bind(VkWriteDescriptorSet* _vkWriteDescriptorSet,
    const VkDescriptorSet& _vkDescriptorSet, uint32_t _dstBinding, uint32_t _dstArrayElement, uint32_t _descriptorCount,
    const VkDescriptorBufferInfo& bufferInfo) const
{
    _vkWriteDescriptorSet->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    _vkWriteDescriptorSet->dstSet = _vkDescriptorSet;
    _vkWriteDescriptorSet->dstBinding = _dstBinding;
    _vkWriteDescriptorSet->dstArrayElement = _dstArrayElement;
    _vkWriteDescriptorSet->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    _vkWriteDescriptorSet->descriptorCount = _descriptorCount;
    _vkWriteDescriptorSet->pBufferInfo = &bufferInfo;
}
