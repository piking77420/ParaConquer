#include "pch.h"
#include "back_end/vulkan_main.h"

#include "front_end/vulkan_app.hpp"

VK_NP::VulkanMain::VulkanMain(const VulkanAppCreateInfo& vulkanMainCreateInfo) :
    m_vulkanHardwareWrapper(vulkanMainCreateInfo)
    , m_vulkanPresentChain(vulkanMainCreateInfo)
{
    m_SwapChainCommandFactory = VulkanCommandFactory(m_VulkanContext.queuFamiliesIndicies.graphicsFamily,
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    m_SwapChainCommandFactory.AllocCommandBuffer(m_CommandBuffer.data(), static_cast<uint32_t>(m_CommandBuffer.size()),
        vk::CommandBufferLevel::ePrimary);
}

VK_NP::VulkanMain::~VulkanMain()
{
}

#pragma region Shader

void VK_NP::VulkanMain::CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo,
                                     const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    m_vulkanShaderManager.CreateShaderFromSource(programShaderCreateInfo, _shaderSource);
}

void VK_NP::VulkanMain::PushConstant(const std::string& _shaderName, const char* pushConstantName, const void* _value,
    size_t _size)
{
   
    assert(m_BindCommandBuffer != VK_NULL_HANDLE, "Try to push constant but bindedcommandBuffer is VK_NULL_HANDLE");
    
    m_vulkanShaderManager.PushConstant(_shaderName, pushConstantName, _value, _size, m_BindCommandBuffer);
}

bool VK_NP::VulkanMain::DestroyShader(const std::string& _shaderName)
{
    return m_vulkanShaderManager.DestroyShader(_shaderName);
}


VULKAN_API void VK_NP::VulkanMain::WaitDevice()
{
    m_VulkanContext.device.waitIdle();
}

void VK_NP::VulkanMain::RecreateSwapChain(void* _glfwWindowPtr, uint32_t _newWidht, uint32_t _newHeight)
{
    m_VulkanContext.swapChainSupportDetails = VK_NP::VulkanPhysicalDevices::QuerySwapChainSupport(
        m_VulkanContext.physicalDevice, m_VulkanContext.surface);
    m_vulkanPresentChain.RecreateSwapChain(_glfwWindowPtr, _newWidht, _newHeight);
}

void VK_NP::VulkanMain::BindCommandBuffer(vk::CommandBuffer _commandBuffer)
{
    m_BindCommandBuffer = _commandBuffer;
}

void VK_NP::VulkanMain::BindProgram(const std::string& _shaderName)
{
    m_vulkanShaderManager.BindProgram(_shaderName, m_CommandBuffer[m_VulkanContext.currentFrame]);
}

#pragma endregion Shader

void VK_NP::VulkanMain::SwapBuffers()
{
    m_vulkanPresentChain.SwapBuffer(&m_VulkanContext);
    m_VulkanContext.currentFrame = (m_VulkanContext.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VK_NP::VulkanMain::BeginDraw()
{
    // wait for new image 
    m_vulkanPresentChain.WaitForAvailableImage(&m_VulkanContext);
    m_vulkanPresentChain.AquireNetImageKHR(&m_VulkanContext);

    // reset command buffer
    BindCommandBuffer(m_CommandBuffer[m_VulkanContext.currentFrame]);
    m_BindCommandBuffer.reset();

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    beginInfo.pInheritanceInfo = nullptr; // Optional

    VK_CALL(m_BindCommandBuffer.begin(&beginInfo));

    // SOUDLE DO IT HERE

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassInfo.renderPass = m_VulkanContext.swapChainRenderPass;
    renderPassInfo.framebuffer = m_VulkanContext.m_SwapChainFramebuffers[m_VulkanContext.imageIndex];

    renderPassInfo.renderArea.offset = vk::Offset2D(0);
    renderPassInfo.renderArea.extent = m_VulkanContext.m_Extent2D;

    vk::ClearValue clearColor = vk::ClearColorValue({0.0f, 0.0f, 0.0f, 1.0f});

    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vk::SubpassContents subpassContents = vk::SubpassContents::eInline;

    m_BindCommandBuffer.beginRenderPass(&renderPassInfo, subpassContents);
}


void VK_NP::VulkanMain::Draw()
{
    BindCommandBuffer(m_CommandBuffer[m_VulkanContext.currentFrame]);

    const vk::Extent2D& extend = m_VulkanContext.m_Extent2D;

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extend.width);
    viewport.height = static_cast<float>(extend.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    m_CommandBuffer[m_VulkanContext.currentFrame].setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D(0, 0);
    scissor.extent = extend;
    m_CommandBuffer[m_VulkanContext.currentFrame].setScissor(0, 1, &scissor);

    m_CommandBuffer[m_VulkanContext.currentFrame].draw(3, 1, 0, 0);
}

void VK_NP::VulkanMain::EnDraw()
{
    BindCommandBuffer(m_CommandBuffer[m_VulkanContext.currentFrame]);

    m_BindCommandBuffer.endRenderPass();
    m_BindCommandBuffer.end();


    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;


    vk::Semaphore waitSemaphores[] = {
        m_VulkanContext.m_syncObject.at(m_VulkanContext.currentFrame).imageAvailableSemaphore
    };
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_BindCommandBuffer;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_VulkanContext.m_syncObject.at(m_VulkanContext.currentFrame).
                                                    renderFinishedSemaphore;

    VK_CALL(
        m_VulkanContext.graphicQueue.submit(1, &submitInfo, m_VulkanContext.m_syncObject.at(m_VulkanContext.currentFrame
        ).inFlightFence));
}

