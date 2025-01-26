#include "vulkan_command_list.hpp"

#include "vulkan_context.hpp"
#include "vulkan_frame_buffer.hpp"
#include "vulkan_render_pass.hpp"


Vulkan::VulkanCommandList::VulkanCommandList(PC_CORE::CommandPoolFamily _commandPoolFamily)
: CommandList(_commandPoolFamily)
{
    VulkanContext& vulkanContext = VulkanContext::GetContext();
    vk::CommandPool commandPool = VK_NULL_HANDLE;
    
    switch (m_CommandPoolFamily)
    {
    case PC_CORE::CommandPoolFamily::Graphics:
        commandPool = vulkanContext.commandPool;
        break;
    case PC_CORE::CommandPoolFamily::Compute:
        break;
    case PC_CORE::CommandPoolFamily::Count:
        break;
    default: ;
    }
    
    if (commandPool == VK_NULL_HANDLE)
    {
        // TO DO GET CALL FOR LOGGING
        return;
    }
    
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = 1;

    auto commandBuffers = vulkanContext.GetDevice()->GetDevice().allocateCommandBuffers(commandPool, &commandBufferAllocateInfo);
    m_CommandBuffer = commandBuffers[0];
    
}

Vulkan::VulkanCommandList::~VulkanCommandList()
{
    
}

void Vulkan::VulkanCommandList::BeginRecordCommands()
{
    if (m_CommandBuffer == VK_NULL_HANDLE)
        return;
    
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits(); // Optional
    commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

    m_CommandBuffer.begin(commandBufferBeginInfo);
}

void Vulkan::VulkanCommandList::BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo)
{
    std::shared_ptr<const VulkanFrameBuffer> frameBuffer = std::reinterpret_pointer_cast<const  VulkanFrameBuffer>(_BeginRenderPassInfo.frameBuffer);
    std::shared_ptr<const  VulkanRenderPass> renderPass = std::reinterpret_pointer_cast<const  VulkanRenderPass>(_BeginRenderPassInfo.renderPass);

    
    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassInfo.renderPass = renderPass->GetVulkanRenderPass();
    renderPassInfo.framebuffer = frameBuffer->GetFramebuffer();
    renderPassInfo.renderArea.offset = vk::Offset2D{_BeginRenderPassInfo.renderOffSet.x, _BeginRenderPassInfo.renderOffSet.y};
    renderPassInfo.renderArea.extent = vk::Extent2D{_BeginRenderPassInfo.extent.x, _BeginRenderPassInfo.extent.y};

    vk::ClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    m_CommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void Vulkan::VulkanCommandList::EndRenderPass()
{
    m_CommandBuffer.endRenderPass();
}
    
