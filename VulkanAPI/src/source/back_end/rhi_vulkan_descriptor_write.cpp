#include "back_end/rhi_vulkan_descriptor_write.hpp"

#include "rhi_vulkan_parser.hpp"

vk::WriteDescriptorSet Vulkan::Backend::RhiToVulkanWriteDescriptorSet(
    const PC_CORE::DescriptorWriteSet& _writeDescriptorSet,
    vk::DescriptorBufferInfo* _bufferInfo, vk::DescriptorImageInfo* _imageInfo, vk::BufferView* _pTexelBufferView)
{
    vk::WriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = vk::StructureType::eWriteDescriptorSet;
    descriptorWrite.dstSet = CastObjectToVkObject<vk::DescriptorSet>(_writeDescriptorSet.dstDescriptorSetHandle);
    descriptorWrite.dstBinding = _writeDescriptorSet.dstBinding;
    descriptorWrite.dstArrayElement = _writeDescriptorSet.dstArrayElement;
    descriptorWrite.descriptorType = RHIDescriptorTypeToVulkan(_writeDescriptorSet.descriptorType);
    descriptorWrite.descriptorCount = _writeDescriptorSet.descriptorCount;

    if (_writeDescriptorSet.descriptorBufferInfo != nullptr)
    {
        *_bufferInfo = RhiToVulkanDescriptorBufferInfo(*_writeDescriptorSet.descriptorBufferInfo);
        descriptorWrite.pBufferInfo = _bufferInfo;
    }

    if (_writeDescriptorSet.descriptorImageInfo != nullptr)
    {
        // TODO DO IMAGE 
    }

    if (_writeDescriptorSet.descriptorTexelBufferViewInfo != nullptr)
    {
        // TODO DO descriptorTexelBufferViewInfo 
    }


    return descriptorWrite;
}

vk::DescriptorBufferInfo Vulkan::Backend::RhiToVulkanDescriptorBufferInfo(
    const PC_CORE::DescriptorBufferInfo& _descriptorBufferInfo)
{
    vk::DescriptorBufferInfo descriptorBuffer;
    descriptorBuffer.buffer = CastObjectToVkObject<vk::Buffer>(_descriptorBufferInfo.buffer);
    descriptorBuffer.offset = _descriptorBufferInfo.offset;
    descriptorBuffer.range = _descriptorBufferInfo.range;

    return descriptorBuffer;
}
