#include "VulkanFrameBuffer.hpp"

#include "VulkanContext.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanTexture.hpp"
#include "LowRenderer/Rhi.hpp"

bool Vulkan::VulkanFrameBuffer::Build()
{
    assert(false); // TODO
    return false;
}

Vulkan::VulkanFrameBuffer::VulkanFrameBuffer(PC_CORE::Rhi& _Rhi, const std::string& _name, const PC_CORE::CreateFrameInfo& _createFrameInfo)
    : RhiFrameBuffer(_Rhi, _name, _createFrameInfo.Width, _createFrameInfo.Height)
{
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(
        GET_VK_CONTEXT.rhiDevice);

    auto renderPass = reinterpret_cast<const VulkanRenderPass*>(_createFrameInfo.RenderPass);

    int frame = 0;
    for (auto& framebuffer : m_FrameBuffers)
    {
        std::vector<vk::ImageView> image_views;
        image_views.reserve(_createFrameInfo.Attachements->size());


        for (auto& attachement : *_createFrameInfo.Attachements)
        {
            const TextureAndAlloc* textureAndAlloc = static_cast<const TextureAndAlloc*>(attachement.RhiTexture->
                GetFrameNativeHandle(frame));

            image_views.emplace_back(textureAndAlloc->ImageView);
        }

        vk::FramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = vk::StructureType::eFramebufferCreateInfo;
        framebufferCreateInfo.flags = {};
        framebufferCreateInfo.renderPass = renderPass->GetVulkanRenderPass();
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(image_views.size());
        framebufferCreateInfo.pAttachments = image_views.data();
        framebufferCreateInfo.width = _createFrameInfo.Width;
        framebufferCreateInfo.height = _createFrameInfo.Height;
        framebufferCreateInfo.layers = 1;
        framebuffer = vulkanDevice->GetDevice().createFramebuffer(framebufferCreateInfo);

        frame++;
    }
}


Vulkan::VulkanFrameBuffer::VulkanFrameBuffer(PC_CORE::Rhi& _Rhi, const std::string& _name, vk::Framebuffer _vkFramebuffer, const uint32_t _widht,
                                             const uint32_t _height) 
    : RhiFrameBuffer(_Rhi, _name, _widht, _height)
{
    for (auto& framebuffer : m_FrameBuffers)
    {
        framebuffer = _vkFramebuffer;
    }
}

Vulkan::VulkanFrameBuffer::~VulkanFrameBuffer()
{
    for (auto& framebuffer : m_FrameBuffers)
    {
        if (framebuffer == VK_NULL_HANDLE)
            continue;

        std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(
            GET_VK_CONTEXT.rhiDevice);
        vulkanDevice->GetDevice().destroyFramebuffer(framebuffer);
        framebuffer = VK_NULL_HANDLE;
    }
}

vk::Framebuffer Vulkan::VulkanFrameBuffer::GetFramebuffer() const
{
    return m_FrameBuffers[m_Rhi.GetFrameIndex()];
}
