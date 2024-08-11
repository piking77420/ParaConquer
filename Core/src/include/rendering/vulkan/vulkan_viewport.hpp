#pragma once

#include "vulkan_header.h"
#include "vulkan_render_pass.hpp"
#include "vulkan_texture.hpp"
#include "math/toolbox_typedef.hpp"

namespace PC_CORE
{
    class Renderer;
}

BEGIN_PCCORE

struct ForwardAttachment
{
    VulkanTexture colorImage;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    //VkDescriptorSet drawQuadDescriptorSet;
};
struct ViewPortFinalImage
{
    VulkanTexture colorImage;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
};
struct ViewPort
{
    Tbx::Vector2i viewPortSize = {800,600};
    std::vector<ForwardAttachment> forwardAttachments;
    std::vector<ViewPortFinalImage> viewPortFinalImageAttachment;
    VulkanTexture depthAttachment;
    bool isEditor = true;
};
    

class VulkanViewport
{
public:
    void Init(Renderer* renderer);

    uint32_t CreateViewPort(bool isIsEditor);

    void DestroyViewPort(uint32_t _viewpPortid);

    const ViewPort& GetViewPort(uint32_t _id) const;

    void Destroy();
    
    bool OnResize(uint32_t viewportId, Tbx::Vector2i _windowSize);
    
protected:
    std::map<uint32_t,ViewPort*> m_ViewPorts;

    Renderer* m_RendererRef = nullptr;
    
    void CreateViewPortImageAndFrameBuffer(ViewPort* viewPort);

    void DestroyViewPortImageAndFrameBuffer(ViewPort* viewPort);

    void InitForward(ViewPort* viewPort);

    void DestroyForward(ViewPort* viewPort);

    void InitFinalImage(ViewPort* viewPort);

    void DestroyFinalImage(ViewPort* viewPort);
};

END_PCCORE