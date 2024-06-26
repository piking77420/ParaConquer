﻿#include "rendering/vulkan/vulkan_viewport.hpp"

#include "app.hpp"
#include "log.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/renderer.hpp"

using namespace PC_CORE;

void VulkanViewport::Init()
{
    const uint32_t nbrOfImage = VulkanInterface::GetNbrOfImage(); 
    forwardAttachments.resize(nbrOfImage);
    viewPortFinalImageAttachment.resize(nbrOfImage);
    
    CreateViewPortImageAndFrameBuffer();
}

void VulkanViewport::Destroy()
{
    DestroyViewPortImageAndFrameBuffer();
}

bool VulkanViewport::OnResize(Vector2i _windowSize)
{
    viewPortSize = _windowSize;
    DestroyViewPortImageAndFrameBuffer();
    CreateViewPortImageAndFrameBuffer();

    return true;
}

void VulkanViewport::CreateViewPortImageAndFrameBuffer()
{
    InitForward();
}

void VulkanViewport::DestroyViewPortImageAndFrameBuffer()
{
    DestroyForward();
}

void VulkanViewport::InitForward()
{
    const uint32_t nbrOfImage = VulkanInterface::GetNbrOfImage(); 


    VkImageCreateInfo  vkImageCreateInfoColor
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VulkanInterface::vulkanSwapChapchain.surfaceFormatKhr.format,
            .extent = {static_cast<uint32_t>(viewPortSize.x), static_cast<uint32_t>(viewPortSize.y),static_cast<uint32_t>(1)},
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
            .extent = {static_cast<uint32_t>(viewPortSize.x), static_cast<uint32_t>(viewPortSize.y),static_cast<uint32_t>(1)},
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


    depthAttachment.Init(vkImageCreateInfoDepth, VK_IMAGE_ASPECT_DEPTH_BIT , VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    for (size_t i = 0 ; i < forwardAttachments.size(); i++)
    {
        ForwardAttachment& fwdAtt = forwardAttachments[i];
        
        fwdAtt.colorImage.Init(vkImageCreateInfoColor, VK_IMAGE_ASPECT_COLOR_BIT , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        const std::array<VkImageView,2> attachments =
        {
            fwdAtt.colorImage.textureImageView,
            depthAttachment.textureImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = App::instance->renderer.renderPasses.at(FORWARD).renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = viewPortSize.x;
        framebufferInfo.height = viewPortSize.y;
        framebufferInfo.layers = 1;

        const VkResult res = vkCreateFramebuffer(VulkanInterface::GetDevice().device, &framebufferInfo, nullptr, &fwdAtt.framebuffer);
        VK_CHECK_ERROR(res,"failed to create framebuffer!");

        //fwdAtt.drawQuadDescriptorSet = renderer->drawQuad.DrawQuadAddTexture(fwdAtt.colorImage); 
    }  
    
}

void VulkanViewport::DestroyForward()
{
    depthAttachment.Destroy();

    for (size_t i = 0 ; i < forwardAttachments.size(); i++)
    {
        ForwardAttachment& fwdAtt = forwardAttachments[i];
        fwdAtt.colorImage.Destroy();
        //renderer->drawQuad.FreeDescriptorSet(fwdAtt.drawQuadDescriptorSet); 
        vkDestroyFramebuffer(VulkanInterface::GetDevice().device, fwdAtt.framebuffer, nullptr);
    }
}

void VulkanViewport::InitFinalImage()
{
    const uint32_t nbrOfImage = VulkanInterface::GetNbrOfImage();
    VkImageCreateInfo  vkImageCreateInfoColor
     {
         .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
         .pNext = nullptr,
         .flags = 0,
         .imageType = VK_IMAGE_TYPE_2D,
         .format = VulkanInterface::vulkanSwapChapchain.surfaceFormatKhr.format,
         .extent = {static_cast<uint32_t>(viewPortSize.x), static_cast<uint32_t>(viewPortSize.y),static_cast<uint32_t>(1)},
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

    for (size_t i = 0 ; i < viewPortFinalImageAttachment.size(); i++)
    {
        ViewPortFinalImage& viewPortFinalImage = viewPortFinalImageAttachment[i];
        
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
        framebufferInfo.width = viewPortSize.x;
        framebufferInfo.height = viewPortSize.y;
        framebufferInfo.layers = 1;

        const VkResult res = vkCreateFramebuffer(VulkanInterface::GetDevice().device, &framebufferInfo, nullptr, &viewPortFinalImage.framebuffer);
        VK_CHECK_ERROR(res,"failed to create framebuffer!");
    }  
}

void VulkanViewport::DestroyFinalImage()
{
    for (size_t i = 0 ; i < viewPortFinalImageAttachment.size(); i++)
    {
        ViewPortFinalImage& viewPortFinalImage = viewPortFinalImageAttachment[i];
        viewPortFinalImage.colorImage.Destroy();
        vkDestroyFramebuffer(VulkanInterface::GetDevice().device, viewPortFinalImage.framebuffer, nullptr);
    }
}
