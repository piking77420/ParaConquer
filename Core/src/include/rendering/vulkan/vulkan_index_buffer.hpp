#pragma once

#include "vulkan_buffer.hpp"
#include "vulkan_header.h"

BEGIN_PCCORE

class VulkanIndexBuffer : public VulkanBuffer
{
public:
    void Init(const std::vector<uint32_t>& _indicies);
    
};

END_PCCORE