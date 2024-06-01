#pragma once

#include "vulkan_header.h"

#include "rendering/vertex.hpp"

BEGIN_PCCORE

class VertexBuffer
{
public:
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

    void Init(const std::vector<Vertex>& vertices);

    void Destroy();
};

END_PCCORE