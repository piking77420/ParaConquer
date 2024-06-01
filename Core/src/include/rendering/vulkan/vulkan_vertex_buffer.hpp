#pragma once

#include "vulkan_buffer.hpp"
#include "vulkan_header.h"

#include "rendering/vertex.hpp"

BEGIN_PCCORE

class VulkanVertexBuffer : public VulkanBuffer
{
public:
    void Init(const std::vector<Vertex>& vertices);
    
};

END_PCCORE