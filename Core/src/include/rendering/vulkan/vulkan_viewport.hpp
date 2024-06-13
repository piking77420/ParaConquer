﻿#pragma once

#include "vulkan_header.h"
#include "vulkan_render_pass.hpp"
#include "vulkan_texture.hpp"
#include "math/toolbox_typedef.hpp"

namespace PC_CORE
{
    class Renderer;
}

BEGIN_PCCORE
    class VulkanViewport
{
private:
    struct ForwardAttachment
    {
        VulkanTexture colorImage;
        VulkanTexture depth;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
    };
    
public:
    Vector2i viewPortSize = {800,600};
    
    std::vector<ForwardAttachment> forwardAttachments;  

    std::vector<VkDescriptorSet> forwardDescritporSet;
    
    Renderer* renderer = nullptr;
    
    bool isEditor = true;

    void Init();

    void Destroy();
    
    bool OnResize(Vector2i _windowSize);
protected:
    
    void CreateViewPortImageAndFrameBuffer();

    void DestroyViewPortImageAndFrameBuffer();

    void InitForward();

    void DestroyForward();
};

END_PCCORE