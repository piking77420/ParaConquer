#include "back_end/rhi_vulkan_descriptor_layout.hpp"

#include "rhi_vulkan_parser.hpp"



vk::DescriptorSetLayoutBinding VK_NP::Backend::RHiDescriptorLayoutBindingToVulkan(const PC_CORE::DescriptorLayoutBinding& _descriptorLayoutBinding)
{
    vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding = _descriptorLayoutBinding.binding;
    descriptorSetLayoutBinding.descriptorCount = _descriptorLayoutBinding.descriptorCount;
    descriptorSetLayoutBinding.descriptorType = RHIDescriptorTypeToVulkan(_descriptorLayoutBinding.descriptorType);
    descriptorSetLayoutBinding.stageFlags = RHIShaderStageToVulkan(_descriptorLayoutBinding.shaderStages);
    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

    return descriptorSetLayoutBinding;
}

vk::DescriptorSetLayout VK_NP::Backend::CreateDescriptorSetLayout(vk::Device _device, const std::vector<PC_CORE::DescriptorLayoutBinding>& _bindings)
{
    std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings(_bindings.size());
    for (size_t i = 0; i < _bindings.size(); i++)
    {
        descriptorSetLayoutBindings[i] = VK_NP::Backend::RHiDescriptorLayoutBindingToVulkan(_bindings[i]);
    }

    vk::DescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();
    
    VK_CALL(_device.createDescriptorSetLayout(&descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));

    return descriptorSetLayout;
}

void VK_NP::Backend::DestroyDescriptorSetLayout(vk::Device _device, vk::DescriptorSetLayout _descriptorSetLayout)
{
    _device.destroyDescriptorSetLayout(_descriptorSetLayout);
}
