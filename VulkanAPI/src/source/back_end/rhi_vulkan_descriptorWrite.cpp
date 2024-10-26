#include "back_end/rhi_vulkan_descriptorWrite.hpp"

#include "rhi_vulkan_parser.hpp"

vk::WriteDescriptorSet VK_NP::Backend::RhiToVulkanWriteDescriptorSet(
    const PC_CORE::DescriptorWriteSet& _writeDescriptorSet)
{
    vk::WriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = vk::StructureType::eWriteDescriptorSet;
    descriptorWrite.dstSet = CastObjectToVkObject<vk::DescriptorSet>(_writeDescriptorSet.dstDescriptorSetHandle);
    descriptorWrite.dstBinding = _writeDescriptorSet.dstBinding;
    descriptorWrite.dstArrayElement = _writeDescriptorSet.dstArrayElement;
    descriptorWrite.descriptorType = RHIDescriptorTypeToVulkan(_writeDescriptorSet.descriptorType);
    
    
}

vk::DescriptorBufferInfo VK_NP::Backend::RhiToVulkanDescriptorBufferInfo(
    const PC_CORE::DescriptorBufferInfo& _descriptorBufferInfo)
{
    const vk::DescriptorBufferInfo descriptorBuffer =
        {
        .buffer = CastObjectToVkObject<vk::Buffer>(_descriptorBufferInfo.buffer),
        .offset = _descriptorBufferInfo.offset,
        .range = _descriptorBufferInfo.range
        };

    return descriptorBuffer;
}
