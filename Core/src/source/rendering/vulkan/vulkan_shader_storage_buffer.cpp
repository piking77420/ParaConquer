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
