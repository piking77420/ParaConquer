#pragma once

#include "vulkan_header.h"

namespace Vulkan
{
    namespace Backend
    {
        vk::WriteDescriptorSet RhiToVulkanWriteDescriptorSet(const PC_CORE::DescriptorWriteSet& _writeDescriptorSet,
            std::vector<vk::DescriptorBufferInfo>* _descriptorBufferInfo, std::vector<vk::DescriptorImageInfo>* _imageInfo, std::vector<vk::BufferView>* _pTexelBufferView);


        vk::DescriptorBufferInfo RhiToVulkanDescriptorBufferInfo(const PC_CORE::DescriptorBufferInfo& _descriptorBufferInfo);

        vk::DescriptorImageInfo rhiToVulkanDescriptorImageInfo(const PC_CORE::DescriptorImageInfo& _descriptorImageInfo);
        
        
    }
}
