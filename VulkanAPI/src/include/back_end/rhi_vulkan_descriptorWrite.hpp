#pragma once

#include "vulkan_header.h"

namespace VK_NP
{
    namespace Backend
    {
        vk::WriteDescriptorSet RhiToVulkanWriteDescriptorSet(const PC_CORE::DescriptorWriteSet& _writeDescriptorSet);


        vk::DescriptorBufferInfo RhiToVulkanDescriptorBufferInfo(const PC_CORE::DescriptorBufferInfo& _descriptorBufferInfo);
        
    }
}
