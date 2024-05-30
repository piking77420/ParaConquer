#include "renderer.hpp"

using namespace PC_CORE;

void Renderer::Init(GLFWwindow* _window)
{
    m_VulkanInterface.Init(_window);
    
    m_CommandBuffers.resize(m_VulkanInterface.nbrOfImage);
    m_VulkanInterface.AllocateCommandBuffers(m_VulkanInterface.nbrOfImage, m_CommandBuffers.data());
}

void Renderer::Destroy()
{
    m_VulkanInterface.Destroy();
}

void Renderer::BeginCommandBuffer(VkCommandBuffer _commandBuffer, VkCommandBufferUsageFlags _usageFlags)
{
    VkCommandBufferBeginInfo  vkCommandBufferBeginInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = _usageFlags,
        .pInheritanceInfo = nullptr
    };
    
    vkBeginCommandBuffer(_commandBuffer, &vkCommandBufferBeginInfo);
}

void Renderer::RecordCommandBuffers()
{
    const VkClearColorValue clearColorValue = { 1.f,1.f ,1.f, 1.f};

    VkImageSubresourceRange vkImageSubresource =
    {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };

    for (size_t i = 0; i < m_CommandBuffers.size(); i++)
    {
        
    }
    
    
}
