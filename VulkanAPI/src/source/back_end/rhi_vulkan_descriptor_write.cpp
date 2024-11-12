#include "back_end/rhi_vulkan_descriptor_write.hpp"

#include "rhi_vulkan_parser.hpp"

vk::WriteDescriptorSet Vulkan::Backend::RhiToVulkanWriteDescriptorSet(
    const PC_CORE::DescriptorWriteSet& _writeDescriptorSet,
    std::vector<vk::DescriptorBufferInfo>* _descriptorBufferInfo, std::vector<vk::DescriptorImageInfo>* _imageInfo, std::vector<vk::BufferView>* _pTexelBufferView)
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
        _descriptorBufferInfo->push_back(RhiToVulkanDescriptorBufferInfo(*_writeDescriptorSet.descriptorBufferInfo));
        descriptorWrite.pBufferInfo = &_descriptorBufferInfo->at(_descriptorBufferInfo->size() - 1);
    }

    if (_writeDescriptorSet.descriptorImageInfo != nullptr)
    {
       _imageInfo->push_back(rhiToVulkanDescriptorImageInfo(*_writeDescriptorSet.descriptorImageInfo));
        descriptorWrite.pImageInfo = &_imageInfo->at(_imageInfo->size() - 1);
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

vk::DescriptorImageInfo Vulkan::Backend::rhiToVulkanDescriptorImageInfo(
    const PC_CORE::DescriptorImageInfo& _descriptorImageInfo)
{
    vk::DescriptorImageInfo descriptorImage = {};

    descriptorImage.sampler = CastObjectToVkObject<vk::Sampler>(_descriptorImageInfo.sampler),
        descriptorImage.imageView = CastObjectToVkObject<vk::ImageView>(_descriptorImageInfo.imageView),
        descriptorImage.imageLayout = RHIToVKImageLayout(_descriptorImageInfo.imageLayout);

    return descriptorImage;
}
