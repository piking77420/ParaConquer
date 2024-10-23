#include "back_end/rhi_vulkan_descriptor_layout.hpp"

#include "rhi_vulkan_parser.hpp"

vk::DescriptorSetLayoutBinding VK_NP::RHiDescriptorLayoutBindingToVulkan(const PC_CORE::DescriptorLayoutBinding& _descriptorLayoutBinding)
{
    vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding = _descriptorLayoutBinding.binding;
    descriptorSetLayoutBinding.descriptorCount = _descriptorLayoutBinding.descriptorCount;
    descriptorSetLayoutBinding.descriptorType = RHIDescriptorTypeToVulkan(_descriptorLayoutBinding.descriptorType);
    descriptorSetLayoutBinding.stageFlags 
}

