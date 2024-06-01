#include "rendering/renderer.hpp"

#include <string>

#include "rendering/vertex.hpp"
#include "app.hpp"
#include "log.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;

void Renderer::Init(Window* _window)
{
    m_VulkanInterface.Init(_window);
    
    m_CommandBuffers.SetNbrofAllocation(VulkanInterface::GetNbrOfImage());
    m_VulkanInterface.AllocateCommandBuffers(VulkanInterface::GetNbrOfImage(), m_CommandBuffers.GetPtr());

    const ShaderSource* vertex = ResourceManager::Get<ShaderSource>("shader_base.vert");
    const ShaderSource* frag = ResourceManager::Get<ShaderSource>("shader_base.frag");

    m_VulkanShaderStage.Init({ vertex, frag });
    CreateBasicGraphiPipeline();
    CreateAsyncObject();
    m_VertexBuffer.Init(vertices);
}

void Renderer::RecreateSwapChain(Window* _window)
{
    vkDeviceWaitIdle(VulkanInterface::GetDevice().device);

    m_VulkanInterface.RecreateSwapChain(_window);
}

void Renderer::Destroy()
{
    // Wait the gpu 
    vkDeviceWaitIdle(VulkanInterface::GetDevice().device);
    
    DestroyAsyncObject();
    m_VertexBuffer.Destroy();
    m_VulkanInterface.Destroy();
}
// to do put fence and semaphore in swap chain
void Renderer::RenderViewPort()
{
    const VkDevice& device = VulkanInterface::GetDevice().device;

    vkWaitForFences(device, 1, &m_InFlightFence[VulkanInterface::GetCurrentFrame()], VK_TRUE, UINT64_MAX);
    
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, m_VulkanInterface.vulkanSwapChapchain.swapchainKhr, UINT64_MAX, m_ImageAvailableSemaphore[VulkanInterface::GetCurrentFrame()], VK_NULL_HANDLE, &imageIndex);
    
    vkResetFences(device, 1, &m_InFlightFence[VulkanInterface::GetCurrentFrame()]);

    vkResetCommandBuffer(m_CommandBuffers[VulkanInterface::GetCurrentFrame()],0);
    RecordCommandBuffers(m_CommandBuffers[VulkanInterface::GetCurrentFrame()], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


    const VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphore[VulkanInterface::GetCurrentFrame()]};
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffers[VulkanInterface::GetCurrentFrame()];

    const VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphore[VulkanInterface::GetCurrentFrame()]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    

    if (vkQueueSubmit(VulkanInterface::GetDevice().graphicsQueue.Queue, 1, &submitInfo, m_InFlightFence[VulkanInterface::GetCurrentFrame()]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    const VkSwapchainKHR swapChains[] = {m_VulkanInterface.vulkanSwapChapchain.swapchainKhr};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

     vkQueuePresentKHR(VulkanInterface::GetDevice().graphicsQueue.Queue, &presentInfo);
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

    const VkResult r = vkBeginCommandBuffer(_commandBuffer, &vkCommandBufferBeginInfo);

    VK_CHECK_ERROR(r,"Failed to begin CommandBuffer")
}

void Renderer::RecordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginCommandBuffer(commandBuffer,0);


    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_VulkanInterface.vulkanSwapChapchain.mainRenderPass.renderPass;
    renderPassInfo.framebuffer = m_VulkanInterface.GetSwapChainFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_VulkanInterface.vulkanSwapChapchain.swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_BasePipeline.pipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_VulkanInterface.vulkanSwapChapchain.swapChainExtent.width);
    viewport.height = static_cast<float>(m_VulkanInterface.vulkanSwapChapchain.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_VulkanInterface.vulkanSwapChapchain.swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);            

    VkBuffer vertexBuffers[] = {m_VertexBuffer.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    
    vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    
    const VkResult r = vkEndCommandBuffer(commandBuffer);
    VK_CHECK_ERROR(r,"Failed to begin EndCommandBuffer")
    
}

void Renderer::CreateBasicGraphiPipeline()
{
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkResult res = vkCreatePipelineLayout(VulkanInterface::GetDevice().device, &pipelineLayoutInfo, nullptr, &VkPipelineLayout);
    VK_CHECK_ERROR(res,"Failed To create GraphicPipeline")

    /////////////////////////
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    m_BasePipeline.Init(&pipelineInfo, m_VulkanShaderStage, VkPipelineLayout, m_VulkanInterface.vulkanSwapChapchain.mainRenderPass.renderPass);
    
}

void Renderer::CreateAsyncObject()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    // make the fence to not block at the start of the first frame to avoid to wait the fence infinitely
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    m_ImageAvailableSemaphore.Init(semaphoreInfo, VulkanInterface::GetNbrOfImage());
    m_RenderFinishedSemaphore.Init(semaphoreInfo, VulkanInterface::GetNbrOfImage());
    m_InFlightFence.Init(fenceInfo, VulkanInterface::GetNbrOfImage());
}

void Renderer::DestroyAsyncObject()
{
    m_RenderFinishedSemaphore.Destroy();
    m_InFlightFence.Destroy();
    m_ImageAvailableSemaphore.Destroy();
}