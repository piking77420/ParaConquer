#include "rendering/renderer.hpp"

#include <string>

#include "rendering/vertex.hpp"
#include "app.hpp"
#include "log.hpp"
#include "math/matrix_transformation.hpp"
#include "rendering/vulkan/vulkan_texture_sampler.hpp"
#include "resources/resource_manager.hpp"

using namespace PC_CORE;

void Renderer::Init(Window* _window)
{
    m_CommandBuffers.resize(VulkanInterface::GetNbrOfImage());
    VulkanInterface::vulkanCommandPoolGraphic.AllocCommandBuffer(VulkanInterface::GetNbrOfImage(), m_CommandBuffers.data());

    const ShaderSource* vertex = ResourceManager::Get<ShaderSource>("shader_base.vert");
    const ShaderSource* frag = ResourceManager::Get<ShaderSource>("shader_base.frag");
    
    m_VulkanShaderStage.Init({ vertex, frag });
    diamondtexture = ResourceManager::CreateAndLoad<Texture>("assets/textures/diamond_block.jpg");
    mesh = ResourceManager::Get<Mesh>("cube.obj");
    CreateAsyncObject();
    
    m_UniformBuffers.resize(VulkanInterface::GetNbrOfImage());
    for(VulkanUniformBuffer& uniformBuffer : m_UniformBuffers)
    {
        uniformBuffer.Init(&UniformBufferObject, sizeof(UniformBufferObject));
    }
    
    CreateDescriptorSetLayout();
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateBasicGraphiPipeline();
}

void Renderer::RecreateSwapChain(Window* _window)
{
    vkDeviceWaitIdle(VulkanInterface::GetDevice().device);

    VulkanInterface::RecreateSwapChain(_window);
}

void Renderer::Destroy()
{
    // Wait the gpu 
    vkDeviceWaitIdle(VulkanInterface::GetDevice().device);

    for(VulkanUniformBuffer& uniformBuffer : m_UniformBuffers)
    {
        uniformBuffer.Destroy();
    }
    
    vkDestroyDescriptorPool(VulkanInterface::GetDevice().device, descriptorPool, nullptr);
    m_DescriptorSetLayout.Destroy();

    m_VulkanShaderStage.Destroy();
    m_VkPipelineLayout.Destroy();
    m_BasePipeline.Destroy();
    DestroyAsyncObject();
}
// to do put fence and semaphore in swap chain
void Renderer::RenderViewPort()
{
    const VkDevice& device = VulkanInterface::GetDevice().device;

    vkWaitForFences(device, 1, &m_InFlightFence[VulkanInterface::GetCurrentFrame()].fences, VK_TRUE, UINT64_MAX);
    
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, VulkanInterface::vulkanSwapChapchain.swapchainKhr, UINT64_MAX, m_ImageAvailableSemaphore[VulkanInterface::GetCurrentFrame()].semaphore, VK_NULL_HANDLE, &imageIndex);
    
    vkResetFences(device, 1, &m_InFlightFence[VulkanInterface::GetCurrentFrame()].fences);


    UpdateUniformBuffer(VulkanInterface::GetCurrentFrame());
    
    vkResetCommandBuffer(m_CommandBuffers[VulkanInterface::GetCurrentFrame()],0);
    RecordCommandBuffers(m_CommandBuffers[VulkanInterface::GetCurrentFrame()], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


    const VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphore[VulkanInterface::GetCurrentFrame()].semaphore};
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffers[VulkanInterface::GetCurrentFrame()];

    const VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphore[VulkanInterface::GetCurrentFrame()].semaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    

    if (vkQueueSubmit(VulkanInterface::GetDevice().graphicsQueue.Queue, 1, &submitInfo, m_InFlightFence[VulkanInterface::GetCurrentFrame()].fences) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    const VkSwapchainKHR swapChains[] = {VulkanInterface::vulkanSwapChapchain.swapchainKhr};
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
    renderPassInfo.renderPass = VulkanInterface::vulkanSwapChapchain.mainRenderPass.renderPass;
    renderPassInfo.framebuffer = VulkanInterface::GetSwapChainFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VulkanInterface::vulkanSwapChapchain.swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_BasePipeline.Get());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(VulkanInterface::vulkanSwapChapchain.swapChainExtent.width);
    viewport.height = static_cast<float>(VulkanInterface::vulkanSwapChapchain.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VulkanInterface::vulkanSwapChapchain.swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    const VkBuffer vertexBuffers[] = {mesh->vulkanVertexBuffer.GetHandle()};
    const VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipelineLayout.Get(), 0,
        1, &descriptorSets[VulkanInterface::GetCurrentFrame()], 0, nullptr);
    
    vkCmdBindIndexBuffer(commandBuffer, mesh->vulkanIndexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);
    
    vkCmdDrawIndexed(commandBuffer, mesh->indicies.size(), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    
    const VkResult r = vkEndCommandBuffer(commandBuffer);
    VK_CHECK_ERROR(r,"Failed to begin EndCommandBuffer")
    
}

void Renderer::CreateBasicGraphiPipeline()
{
    std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts =
    {
        m_DescriptorSetLayout.Get()
    };
    
    m_VkPipelineLayout.Init(vkDescriptorSetLayouts);
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
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    
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
    
    VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    m_BasePipeline.Init(&pipelineInfo, m_VulkanShaderStage, m_VkPipelineLayout.Get(),
        VulkanInterface::vulkanSwapChapchain.mainRenderPass.renderPass);
    
}

void Renderer::CreateAsyncObject()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    // make the fence to not block at the start of the first frame to avoid to wait the fence infinitely
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    const size_t nbrOfImage = VulkanInterface::GetNbrOfImage();
    m_ImageAvailableSemaphore.resize(nbrOfImage);
    m_RenderFinishedSemaphore.resize(nbrOfImage);
    m_InFlightFence.resize(nbrOfImage);
    
    for (size_t i = 0; i <  VulkanInterface::GetNbrOfImage(); i++)
    {
        m_ImageAvailableSemaphore[i].Init(semaphoreInfo);
        m_RenderFinishedSemaphore[i].Init(semaphoreInfo);
        m_InFlightFence[i].Init(fenceInfo);
    }
}

void Renderer::DestroyAsyncObject()
{
    const size_t nbrOfImage = VulkanInterface::GetNbrOfImage();
    
    for (size_t i = 0; i <  VulkanInterface::GetNbrOfImage(); i++)
    {
        m_ImageAvailableSemaphore[i].Destroy();
        m_RenderFinishedSemaphore[i].Destroy();
        m_InFlightFence[i].Destroy();
    }
    
}

void Renderer::UpdateUniformBuffer(uint32_t _currentFrame)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    
    Vector3f pos(0.f,0.f,0.f);
    Vector3f rot(0,0.f,90.0f);
    Vector3f scale(1.f,1.f,1.f);
    rot *= time * 0.016f;
    
    Trs3D(pos,rot,scale,&UniformBufferObject.model);
    LookAtRH({2.0f, 2.0f, 2.0f},{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 1.0f},&UniformBufferObject.view);

    float aspect = Window::currentWindow->GetAspect();
    PerspectiveMatrix(45.0f * Deg2Rad,aspect,0.1f,1000.f,&UniformBufferObject.proj);
    UniformBufferObject.proj[1][1] *= -1;

    m_UniformBuffers[_currentFrame].Update(&UniformBufferObject.model.data[0].x,sizeof(UniformBufferObject));
    
}

void Renderer::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = VulkanUniformBuffer::GetLayoutBinding(0,1 ,
        VK_SHADER_STAGE_VERTEX_BIT);
    VkDescriptorSetLayoutBinding samplerLayoutBinding = VulkanTextureSampler::GetDescriptorSetLayoutBinding(1,1,VK_SHADER_STAGE_FRAGMENT_BIT);
    m_DescriptorSetLayout.Init({uboLayoutBinding , samplerLayoutBinding});
}

void Renderer::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(VulkanInterface::GetNbrOfImage());
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(VulkanInterface::GetNbrOfImage());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(VulkanInterface::GetNbrOfImage());

    const VkResult result = vkCreateDescriptorPool(VulkanInterface::GetDevice().device, &poolInfo, nullptr, &descriptorPool);
    VK_CHECK_ERROR(result,"failed to create descriptor pool!");
}
  

void Renderer::CreateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(VulkanInterface::GetNbrOfImage(), m_DescriptorSetLayout.Get());
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanInterface::GetNbrOfImage());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(VulkanInterface::GetNbrOfImage());
    
    if (vkAllocateDescriptorSets(VulkanInterface::GetDevice().device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < VulkanInterface::GetNbrOfImage(); i++)
    {
       
        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};


        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_UniformBuffers[i].GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = diamondtexture->vulkanTexture.textureImageView;
        imageInfo.sampler = VulkanInterface::vulkanTextureSampler.Get(diamondtexture->vulkanTexture.samplerId);
        
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;
        
       vkUpdateDescriptorSets(VulkanInterface::GetDevice().device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}
