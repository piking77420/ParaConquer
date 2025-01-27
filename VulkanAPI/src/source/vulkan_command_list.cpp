#include "vulkan_command_list.hpp"

#include "vulkan_context.hpp"
#include "vulkan_frame_buffer.hpp"
#include "vulkan_render_pass.hpp"
#include "resources/vulkan_shader_program.hpp"


Vulkan::VulkanCommandList::VulkanCommandList(PC_CORE::CommandPoolFamily _commandPoolFamily)
: CommandList(_commandPoolFamily)
{
    VulkanContext& vulkanContext = VulkanContext::GetContext();
    vk::CommandPool commandPool = VK_NULL_HANDLE;
    
    switch (m_CommandPoolFamily)
    {
    case PC_CORE::CommandPoolFamily::Graphics:
        commandPool = vulkanContext.commandPool;
        m_Queue = &vulkanContext.graphicsQueue;
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

   std::vector<vk::CommandBuffer> vec = vulkanContext.GetDevice()->GetDevice().allocateCommandBuffers(commandBufferAllocateInfo);
    m_CommandBuffer = vec[0];
}

Vulkan::VulkanCommandList::~VulkanCommandList()
{
    
}

void Vulkan::VulkanCommandList::Reset()
{
    m_CommandBuffer.reset();
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

void Vulkan::VulkanCommandList::EndRecordCommands()
{
    m_CommandBuffer.end();
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

    vk::ClearValue clearColor  = {};
    clearColor.color.setFloat32({ 0.0f, 0.0f, 0.0f, 1.0f });
    
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    m_CommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void Vulkan::VulkanCommandList::EndRenderPass()
{
    m_CommandBuffer.endRenderPass();
}

void Vulkan::VulkanCommandList::BindProgram(const PC_CORE::ShaderProgram* _shaderProgramm)
{
    const VulkanShaderProgram* vshadeProgram = reinterpret_cast<const VulkanShaderProgram*>(_shaderProgramm); 
    m_CommandBuffer.bindPipeline(vshadeProgram->GetPipelineBindPoint(), vshadeProgram->GetPipeline());
}

void Vulkan::VulkanCommandList::SetViewPort(const PC_CORE::ViewportInfo& _viewPort)
{
    vk::Viewport viewport;
    viewport.x = _viewPort.transform.x;
    viewport.y = _viewPort.transform.y;
    viewport.width = static_cast<float>(_viewPort.size.x);
    viewport.height = static_cast<float>(_viewPort.size.y);
    viewport.minDepth = _viewPort.minDepth;
    viewport.maxDepth = _viewPort.maxDepth;
    m_CommandBuffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor;
    scissor.offset = vk::Offset2D{_viewPort.scissorsOff.x, _viewPort.scissorsOff.y};
    scissor.extent = vk::Extent2D{_viewPort.scissorsextent.x, _viewPort.scissorsextent.y};
    
    m_CommandBuffer.setScissor(0, 1, &scissor);
}

void Vulkan::VulkanCommandList::Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex,
    uint32_t _firstInstance)
{
    m_CommandBuffer.draw(_vertexCount, _instanceCount, _firstVertex, _firstInstance);
}


vk::CommandBuffer Vulkan::VulkanCommandList::GetHandle() const
{
    return m_CommandBuffer;
}

const vk::Queue* Vulkan::VulkanCommandList::GetQueue() const
{
    return m_Queue;
}
    
