#pragma once

#include "vulkan_header.h"

namespace VK_NP
{
    namespace Backend
    {
        vk::DescriptorPoolSize RhiToVkDescriptorPoolSize(const PC_CORE::DesciptorPoolSize& _descriptorPoolSize);

        vk::DescriptorPool RhiToVkDescriptorPool(vk::Device _device, PC_CORE::DesciptorPoolSize* desciptorPoolSize, uint32_t descriptorCount, uint32_t maxSets);

        void DestroyDescriptorPool(vk::Device _device, vk::DescriptorPool _descriptorPool);

    }
}