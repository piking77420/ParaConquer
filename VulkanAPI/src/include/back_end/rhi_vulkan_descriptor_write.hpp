#pragma once

#include "vulkan_header.h"

namespace Vulkan
{
    namespace Backend
    {
        vk::WriteDescriptorSet RhiToVulkanWriteDescriptorSet(const PC_CORE::DescriptorWriteSet& _writeDescriptorSet,
            vk::DescriptorBufferInfo* _bufferInfo, vk::DescriptorImageInfo* _imageInfo, vk::BufferView* _pTexelBufferView);


        vk::DescriptorBufferInfo RhiToVulkanDescriptorBufferInfo(const PC_CORE::DescriptorBufferInfo& _descriptorBufferInfo);

        vk::DescriptorImageInfo rhiToVulkanDescriptorImageInfo(const PC_CORE::DescriptorImageInfo _descriptorImageInfo);
        
        
    }
}
