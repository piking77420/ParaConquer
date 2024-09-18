#include "pch.h"
#include "back_end/vulkan_main.h"

#include "front_end/vulkan_app.hpp"

VK_NP::VulkanMain::VulkanMain(const VulkanAppCreateInfo& vulkanMainCreateInfo) : m_vulkanHardwareWrapper(vulkanMainCreateInfo)
, m_vulkanPresentChain(vulkanMainCreateInfo)
{
    
    CreateCommandPool();
    CreateCommandBuffer();
}

VK_NP::VulkanMain::~VulkanMain()
{
    auto device = m_vulkanHardwareWrapper.GetDevice();
    device.destroyCommandPool(m_CommandPool);

}

#pragma region Shader

void VK_NP::VulkanMain::CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    m_vulkanShaderManager.CreateShaderFromSource(programShaderCreateInfo, _shaderSource);
}

bool VK_NP::VulkanMain::DestroyShader(const std::string& _shaderName)
{
    return m_vulkanShaderManager.DestroyShader(_shaderName);
}

void VK_NP::VulkanMain::BindProgram(const std::string& _shaderName)
{
    m_vulkanShaderManager.BindProgram(_shaderName, m_CommandBuffer);
}

#pragma endregion Shader

void VK_NP::VulkanMain::SwapBuffers()
{
    m_vulkanPresentChain.SwapBuffer();
}

void VK_NP::VulkanMain::BeginDraw()
{
    // wait for new image 
    m_vulkanPresentChain.WaitForAvailableImage();
    m_vulkanPresentChain.AquireNetImageKHR();

    // reset command buffer
    m_CommandBuffer.reset();
    
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    beginInfo.pInheritanceInfo = nullptr; // Optional

    VK_CALL(m_CommandBuffer.begin(&beginInfo));

    // SOUDLE DO IT HERE

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassInfo.renderPass = m_vulkanPresentChain.GetRenderPassTmpr();
    renderPassInfo.framebuffer = m_vulkanPresentChain.GetFramebuffer(imageIndex);

    renderPassInfo.renderArea.offset = vk::Offset2D(0);
    renderPassInfo.renderArea.extent = m_vulkanPresentChain.GetExtent();
    
    vk::ClearValue clearColor = vk::ClearColorValue({ 0.0f, 0.0f, 0.0f, 1.0f });
      
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vk::SubpassContents subpassContents = vk::SubpassContents::eInline;

   m_CommandBuffer.beginRenderPass(&renderPassInfo, subpassContents);
   
}



void VK_NP::VulkanMain::Draw()
{
    auto extend = m_vulkanPresentChain.GetExtent();
    
    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extend.width);
    viewport.height = static_cast<float>(extend.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    m_CommandBuffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D(0,0);
    scissor.extent = extend;
    m_CommandBuffer.setScissor(0, 1, &scissor);

    m_CommandBuffer.draw(3, 1, 0, 0);

}

void VK_NP::VulkanMain::EnDraw()
{
    m_CommandBuffer.endRenderPass();
    m_CommandBuffer.end();


    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;

    vk::Semaphore waitSemaphores[] = { *m_vulkanPresentChain.GetImageAvailableSemaphore() };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer;

    vk::Semaphore signalSemaphores[] = { *m_vulkanPresentChain.GetRenderFinishedSemaphore() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_CALL(m_vulkanHardwareWrapper.GetGraphicQueue().submit(1, &submitInfo, *m_vulkanPresentChain.GetInFlightFence()));

}


void VK_NP::VulkanMain::CreateCommandPool()
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    commandPoolCreateInfo.queueFamilyIndex = m_vulkanHardwareWrapper.GetPhysicalDevices().GetQueueFamiliesIndicies().graphicsFamily;

    VK_CALL(m_vulkanHardwareWrapper.GetDevice().createCommandPool(&commandPoolCreateInfo, nullptr, &m_CommandPool)); 
}

void VK_NP::VulkanMain::CreateCommandBuffer()
{
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;



    VK_CALL(m_vulkanHardwareWrapper.GetDevice().allocateCommandBuffers(&allocInfo, &m_CommandBuffer));
}


