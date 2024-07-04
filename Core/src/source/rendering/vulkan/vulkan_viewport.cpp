#include "rendering/vulkan/vulkan_viewport.hpp"

#include "app.hpp"
#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/renderer.hpp"

using namespace PC_CORE;


void VulkanViewport::Init(Renderer* renderer)
{
    m_RendererRef = renderer;
}

uint32_t VulkanViewport::CreateViewPort(bool isIsEditor)
{
    ViewPort* viewPort = new ViewPort;
    const uint32_t id = m_ViewPorts.size();
    const uint32_t nbrOfImage = VulkanInterface::GetNbrOfImage(); 
    viewPort->forwardAttachments.resize(nbrOfImage);
    viewPort->viewPortFinalImageAttachment.resize(nbrOfImage);
    CreateViewPortImageAndFrameBuffer(viewPort);

    m_ViewPorts.emplace(id, viewPort);
    return id;
}

void VulkanViewport::DestroyViewPort(uint32_t _viewpPortId)
{
    if (!m_ViewPorts.contains(_viewpPortId))
        return;
    
    ViewPort* v = m_ViewPorts.at(_viewpPortId);
    DestroyViewPortImageAndFrameBuffer(v);
    m_ViewPorts.erase(_viewpPortId);
}

const ViewPort& VulkanViewport::GetViewPort(uint32_t _id) const
{
    return *m_ViewPorts.at(_id);
}

void VulkanViewport::Destroy()
{
    for (auto it = m_ViewPorts.begin(); it != m_ViewPorts.end(); it++)
    {
        DestroyViewPortImageAndFrameBuffer(it->second);
        delete it->second;
    }
}

bool VulkanViewport::OnResize(uint32_t viewportId, Vector2i _windowSize)
{
    if (!m_ViewPorts.contains(viewportId))
    {
        return false;
    }

    ViewPort* viewPort = m_ViewPorts.at(viewportId);
    
    viewPort->viewPortSize = _windowSize;
    DestroyViewPortImageAndFrameBuffer(viewPort);
    CreateViewPortImageAndFrameBuffer(viewPort);

    return true;
}

void VulkanViewport::CreateViewPortImageAndFrameBuffer(ViewPort* viewPort)
{
    InitForward(viewPort);
}

void VulkanViewport::DestroyViewPortImageAndFrameBuffer(ViewPort* viewPort)
{
    DestroyForward(viewPort);
}

void VulkanViewport::InitForward(ViewPort* viewPort)
{
    const uint32_t nbrOfImage = VulkanInterface::GetNbrOfImage();
    
    VkImageCreateInfo  vkImageCreateInfoColor
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VulkanInterface::vulkanSwapChapchain.surfaceFormatKhr.format,
            .extent = {static_cast<uint32_t>(viewPort->viewPortSize.x), static_cast<uint32_t>(viewPort->viewPortSize.y),static_cast<uint32_t>(1)},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &VulkanInterface::GetDevice().graphicsQueue.index,
             .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

    VkImageCreateInfo  vkImageCreateInfoDepth
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
           .pNext = nullptr,
           .flags = 0,
           .imageType = VK_IMAGE_TYPE_2D,
           .format = VulkanInterface::vulkanSwapChapchain.depthFormat,
            .extent = {static_cast<uint32_t>(viewPort->viewPortSize.x), static_cast<uint32_t>(viewPort->viewPortSize.y),static_cast<uint32_t>(1)},
           .mipLevels = 1,
           .arrayLayers = 1,
           .samples = VK_SAMPLE_COUNT_1_BIT,
           .tiling = VK_IMAGE_TILING_OPTIMAL,
           .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &VulkanInterface::GetDevice().graphicsQueue.index,
           .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };


    viewPort->depthAttachment.Init(vkImageCreateInfoDepth, VK_IMAGE_ASPECT_DEPTH_BIT , VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    for (size_t i = 0 ; i < viewPort->forwardAttachments.size(); i++)
    {
        ForwardAttachment& fwdAtt = viewPort->forwardAttachments[i];
        
        fwdAtt.colorImage.Init(vkImageCreateInfoColor, VK_IMAGE_ASPECT_COLOR_BIT , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        const std::array<VkImageView,2> attachments =
        {
            fwdAtt.colorImage.textureImageView,
            viewPort->depthAttachment.textureImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = App::instance->renderer.renderPasses.at(FORWARD).renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = viewPort->viewPortSize.x;
        framebufferInfo.height = viewPort->viewPortSize.y;
        framebufferInfo.layers = 1;

        const VkResult res = vkCreateFramebuffer(VulkanInterface::GetDevice().device, &framebufferInfo, nullptr, &fwdAtt.framebuffer);
        VK_CHECK_ERROR(res,"failed to create framebuffer!");

        //fwdAtt.drawQuadDescriptorSet = renderer->drawQuad.DrawQuadAddTexture(fwdAtt.colorImage); 
    }  
    
}

void VulkanViewport::DestroyForward(ViewPort* viewPort)
{
    viewPort->depthAttachment.Destroy();

    for (size_t i = 0 ; i < viewPort->forwardAttachments.size(); i++)
    {
        ForwardAttachment& fwdAtt = viewPort->forwardAttachments[i];
        fwdAtt.colorImage.Destroy();
        //renderer->drawQuad.FreeDescriptorSet(fwdAtt.drawQuadDescriptorSet); 
        vkDestroyFramebuffer(VulkanInterface::GetDevice().device, fwdAtt.framebuffer, nullptr);
    }
}

void VulkanViewport::InitFinalImage(ViewPort* viewPort)
{
    const uint32_t nbrOfImage = VulkanInterface::GetNbrOfImage();
    VkImageCreateInfo  vkImageCreateInfoColor
     {
         .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
         .pNext = nullptr,
         .flags = 0,
         .imageType = VK_IMAGE_TYPE_2D,
         .format = VulkanInterface::vulkanSwapChapchain.surfaceFormatKhr.format,
         .extent = {static_cast<uint32_t>(viewPort->viewPortSize.x), static_cast<uint32_t>(viewPort->viewPortSize.y),static_cast<uint32_t>(1)},
         .mipLevels = 1,
         .arrayLayers = 1,
         .samples = VK_SAMPLE_COUNT_1_BIT,
         .tiling = VK_IMAGE_TILING_OPTIMAL,
         .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
         .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
         .queueFamilyIndexCount = 1,
         .pQueueFamilyIndices = &VulkanInterface::GetDevice().graphicsQueue.index,
          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
     };

    for (size_t i = 0 ; i < viewPort->viewPortFinalImageAttachment.size(); i++)
    {
        ViewPortFinalImage& viewPortFinalImage = viewPort->viewPortFinalImageAttachment[i];
        
        viewPortFinalImage.colorImage.Init(vkImageCreateInfoColor, VK_IMAGE_ASPECT_COLOR_BIT , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        const std::array<VkImageView,2> attachments =
        {
            viewPortFinalImage.colorImage.textureImageView,
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        // COLOR PASS ONLY
        //framebufferInfo.renderPass = App::instance->renderer.renderPasses.at(FORWARD).renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = viewPort->viewPortSize.x;
        framebufferInfo.height = viewPort->viewPortSize.y;
        framebufferInfo.layers = 1;

        const VkResult res = vkCreateFramebuffer(VulkanInterface::GetDevice().device, &framebufferInfo, nullptr, &viewPortFinalImage.framebuffer);
        VK_CHECK_ERROR(res,"failed to create framebuffer!");
    }  
}

void VulkanViewport::DestroyFinalImage(ViewPort* viewPort)
{
    for (size_t i = 0 ; i < viewPort->viewPortFinalImageAttachment.size(); i++)
    {
        ViewPortFinalImage& viewPortFinalImage = viewPort->viewPortFinalImageAttachment[i];
        viewPortFinalImage.colorImage.Destroy();
        vkDestroyFramebuffer(VulkanInterface::GetDevice().device, viewPortFinalImage.framebuffer, nullptr);
    }
}
