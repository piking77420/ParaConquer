#pragma once

#include "vulkan_header.h"
#include "vulkan_texture.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE
class VulkanViewport
{
public:
    Vector2i viewPortSize = {800,600};

    std::vector<VulkanTexture> viewPortTexture;

    std::vector<VkFramebuffer> framebuffers;
    
    VulkanViewport();

    ~VulkanViewport();

    void HandleResize(Vector2i _windowSize);
protected:
    void CreateViewPortImageAndFrameBuffer();

    void DestroyViewPortImageAndFrameBuffer();
};

END_PCCORE