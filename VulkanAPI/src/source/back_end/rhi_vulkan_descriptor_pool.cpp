#include "back_end/rhi_vulkan_descriptor_pool.hpp"
#include "rhi_vulkan_parser.hpp"

vk::DescriptorPoolSize VK_NP::Backend::RhiToVkDescriptorPoolSize(const PC_CORE::DesciptorPoolSize& _descriptorPoolSize)
{
    vk::DescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type = RHIDescriptorTypeToVulkan(_descriptorPoolSize.type);
    descriptorPoolSize.descriptorCount = _descriptorPoolSize.count;

    return descriptorPoolSize;
}

vk::DescriptorPool VK_NP::Backend::RhiToVkDescriptorPool(vk::Device _device, PC_CORE::DesciptorPoolSize* desciptorPoolSize,
    uint32_t descriptorCount, uint32_t maxSets)
{
    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes(static_cast<size_t>(descriptorCount));

    for (uint32_t i = 0; i < descriptorCount; i++)
    {
        descriptorPoolSizes[i] = RhiToVkDescriptorPoolSize(desciptorPoolSize[i]);
    }

    vk::DescriptorPool descriptorPool = VK_NULL_HANDLE;
    vk::DescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolInfo.pPoolSizes = descriptorPoolSizes.data();
    
    VK_CALL(_device.createDescriptorPool(&descriptorPoolInfo, nullptr, &descriptorPool));

    return descriptorPool;
}

void VK_NP::Backend::DestroyDescriptorPool(vk::Device _device, vk::DescriptorPool _descriptorPool)
{
    _device.destroyDescriptorPool(_descriptorPool);
}
