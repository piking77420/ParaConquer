#include "vulkan_frame_buffer.hpp"

#include "vulkan_context.hpp"

Vulkan::VulkanFrameBuffer::VulkanFrameBuffer(vk::Framebuffer _vkFramebuffer, bool _ownBySwapChain) : m_FrameBuffer(_vkFramebuffer) , m_OwnBySwapChain(_ownBySwapChain)
{
        
}

Vulkan::VulkanFrameBuffer::~VulkanFrameBuffer()
{
    if (m_OwnBySwapChain)
        return;

    if (m_FrameBuffer == VK_NULL_HANDLE)
        return;
    
    std::shared_ptr<VulkanDevice> vulkanDevice = std::reinterpret_pointer_cast<VulkanDevice>(VulkanContext::GetContext().rhiDevice); 
    vulkanDevice->GetDevice().destroyFramebuffer(m_FrameBuffer);
}

vk::Framebuffer Vulkan::VulkanFrameBuffer::GetFramebuffer() const
{
    return m_FrameBuffer;
}
