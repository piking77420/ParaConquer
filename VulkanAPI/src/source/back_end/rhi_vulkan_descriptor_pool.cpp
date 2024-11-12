#include "back_end/rhi_vulkan_descriptor_pool.hpp"
#include "rhi_vulkan_parser.hpp"

vk::DescriptorPoolSize Vulkan::Backend::RhiToVkDescriptorPoolSize(const PC_CORE::DescriptorPoolSize& _descriptorPoolSize)
{
    vk::DescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type = RHIDescriptorTypeToVulkan(_descriptorPoolSize.type);
    descriptorPoolSize.descriptorCount = _descriptorPoolSize.count;

    return descriptorPoolSize;
}

vk::DescriptorPool Vulkan::Backend::RhiToVkDescriptorPool(vk::Device _device, const PC_CORE::DescriptorPoolSize* desciptorPoolSize,
    uint32_t _descriptorCount, uint32_t _maxSets)
{
    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes(static_cast<size_t>(_descriptorCount));

    for (uint32_t i = 0; i < _descriptorCount; i++)
    {
        descriptorPoolSizes[i] = RhiToVkDescriptorPoolSize(desciptorPoolSize[i]);
    }

    vk::DescriptorPool descriptorPool = VK_NULL_HANDLE;
    vk::DescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolInfo.pPoolSizes = descriptorPoolSizes.data();
    descriptorPoolInfo.maxSets = _maxSets;
    
    VK_CALL(_device.createDescriptorPool(&descriptorPoolInfo, nullptr, &descriptorPool));

    return descriptorPool;
}

void Vulkan::Backend::DestroyDescriptorPool(vk::Device _device, vk::DescriptorPool _descriptorPool)
{
    _device.destroyDescriptorPool(_descriptorPool);
}
