#include "rendering/vulkan/vulkan_uniform_buffer.h"

#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_wrapper.hpp"

using namespace PC_CORE;

void VulkanUniformBuffer::Init(void const* _data, size_t _size)
{
    CreateBufferVma(&m_Buffer,&m_Allocation, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU,_data, _size);
    vmaMapMemory(VulkanInterface::GetAllocator(), m_Allocation, &m_MapMemory);
    
}

void VulkanUniformBuffer::Update(void const* _data, size_t _size)
{
    memcpy(m_MapMemory, _data, _size);
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
