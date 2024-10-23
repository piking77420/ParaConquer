#pragma once

#include "descriptor_layout_binding.hpp"
#include "vulkan_header.h"

namespace VK_NP
{
    vk::DescriptorSetLayoutBinding RHiDescriptorLayoutBindingToVulkan(const PC_CORE::DescriptorLayoutBinding& _descriptorLayoutBinding);

    
}