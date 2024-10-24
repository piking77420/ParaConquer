#pragma once

#include "descriptor_layout_binding.hpp"
#include "vulkan_header.h"



namespace VK_NP
{
    namespace Backend
    {
        vk::DescriptorSetLayoutBinding RHiDescriptorLayoutBindingToVulkan(const PC_CORE::DescriptorLayoutBinding& _descriptorLayoutBinding);
        
        vk::DescriptorSetLayout CreateDescriptorSetLayout(vk::Device _device, const std::vector<PC_CORE::DescriptorLayoutBinding>& _bindings);

        void DestroyDescriptorSetLayout(vk::Device _device, vk::DescriptorSetLayout _descriptorSetLayout);
    }
   
}