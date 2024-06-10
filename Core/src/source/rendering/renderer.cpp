#include "rendering/renderer.hpp"

#include <string>

#include "rendering/vertex.hpp"
#include "app.hpp"
#include "log.hpp"
#include "math/matrix_transformation.hpp"
#include "rendering/light.hpp"
#include "rendering/vulkan/vulkan_texture_sampler.hpp"
#include "rendering/vulkan/vulkan_viewport.hpp"
#include "resources/resource_manager.hpp"
#include "world/transform.hpp"

using namespace PC_CORE;

void Renderer::Init(Window* _window)
{
    m_GpuLights = new GpuLight;
    const ShaderSource* vertex = ResourceManager::Get<ShaderSource>("shader_base.vert");
    const ShaderSource* frag = ResourceManager::Get<ShaderSource>("shader_base.frag");
    m_VulkanShaderStage.Init({ vertex, frag });
    
    m_MatrixMeshs.resize(MAX_ENTITIES);
    m_CommandBuffers.resize(VulkanInterface::GetNbrOfImage());
    VulkanInterface::vulkanCommandPoolGraphic.AllocCommandBuffer(VulkanInterface::GetNbrOfImage(), m_CommandBuffers.data());
    CreateAsyncObject();
    InitBuffers();
    CreateDescriptorSetLayout();
    CreateDescriptorSets();
    CreateBasicGraphiPipeline();
    drawGizmos.Init(this);
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
    delete m_GpuLights; 
    drawGizmos.Destroy();

    for(VulkanUniformBuffer& uniformBuffer : m_UniformBuffers)
    {
        uniformBuffer.Destroy();
    }
    for(VulkanShaderStorageBuffer& vulkanShaderStorageBuffer : m_ModelMatriciesShaderStorages)
    {
        vulkanShaderStorageBuffer.Destroy();
    }
    for(VulkanShaderStorageBuffer& vulkanShaderStorageBuffer : m_ShaderStoragesLight)
    {
        vulkanShaderStorageBuffer.Destroy();
    }
    
    m_DescriptorSetLayout.Destroy();
    m_DescriptorPool.Destroy();

    m_VulkanShaderStage.Destroy();
    m_VkPipelineLayout.Destroy();
    m_BasePipeline.Destroy();
    DestroyAsyncObject();
}

void Renderer::BeginFrame()
{
    const VkDevice& device = VulkanInterface::GetDevice().device;
    const uint32_t currentFrame = VulkanInterface::GetCurrentFrame();
    vkWaitForFences(device, 1, &m_InFlightFence[currentFrame].fences, VK_TRUE, UINT64_MAX);
    VkResult result = vkAcquireNextImageKHR(device, VulkanInterface::vulkanSwapChapchain.swapchainKhr, UINT64_MAX, m_ImageAvailableSemaphore[VulkanInterface::GetCurrentFrame()].semaphore, VK_NULL_HANDLE, &m_ImageIndex);
    vkResetFences(device, 1, &m_InFlightFence[VulkanInterface::GetCurrentFrame()].fences);
    
    vkResetCommandBuffer(m_CommandBuffers[VulkanInterface::GetCurrentFrame()],0);
}


void Renderer::RenderViewPort(const Camera& _camera, const VulkanViewport& viewport,
    const World& _world)
{
    m_CurrentCamera = &_camera;
    m_CurrentWorld = &_world;
    
    UpdateBuffers(VulkanInterface::GetCurrentFrame());
    RecordCommandBuffers(m_CommandBuffers[VulkanInterface::GetCurrentFrame()], m_ImageIndex);
}

void Renderer::SwapBuffers()
{
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
    presentInfo.pImageIndices = &m_ImageIndex;
    presentInfo.pResults = nullptr; // Optional
    

    vkQueuePresentKHR(VulkanInterface::GetDevice().graphicsQueue.Queue, &presentInfo);
    
    VulkanInterface::ComputeNextFrame();
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
    ForwardPass(commandBuffer, imageIndex);
    
}

void Renderer::ForwardPass(VkCommandBuffer commandBuffer, uint32_t imageIndex)
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

    std::vector<StaticMesh>* meshes;
    m_CurrentWorld->scene.GetComponentData<StaticMesh>(&meshes);
    
    for (size_t i = 0; i < meshes->size(); i++)
    {
        const StaticMesh& staticMesh = meshes->at(i);
        
        if (!IsValid(staticMesh.componentHolder))
            continue;
            
        const Entity& entity = staticMesh.componentHolder.entityID;
        const Transform& transform = *m_CurrentWorld->scene.GetComponent<Transform>(entity);
        DrawStatisMesh(commandBuffer, imageIndex, staticMesh, transform, entity);
    }
    drawGizmos.DrawGizmosForward(commandBuffer, imageIndex);

    
    VulkanImgui::Render(&commandBuffer);
    vkCmdEndRenderPass(commandBuffer);
    const VkResult r = vkEndCommandBuffer(commandBuffer);
    VK_CHECK_ERROR(r,"Failed to begin EndCommandBuffer")
}

void Renderer::CreateBasicGraphiPipeline()
{
    std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts =
    {
        m_DescriptorSetLayout.Get(),
        VulkanInterface::vulkanMaterialManager.descriptorSetLayout.Get()
    };
    VkPushConstantRange pushConstant;
    pushConstant.offset = 0;
    pushConstant.size = sizeof(int32_t);
    pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    m_VkPipelineLayout.Init(vkDescriptorSetLayouts, {pushConstant});
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
    rasterizer.cullMode = VK_CULL_MODE_NONE;
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
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::GetAttributeDescriptions();
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
    
    for (size_t i = 0; i <  nbrOfImage; i++)
    {
        m_ImageAvailableSemaphore[i].Destroy();
        m_RenderFinishedSemaphore[i].Destroy();
        m_InFlightFence[i].Destroy();
    }
    
}

void Renderer::UpdateBuffers(uint32_t _currentFrame)
{
    ComputeModelAndNormalInvertMatrix(_currentFrame);
    UpdateLightBuffer(_currentFrame);
    LookAtRH(m_CurrentCamera->position,m_CurrentCamera->position + m_CurrentCamera->front,m_CurrentCamera->up, &cameraBuffer.view);
    const float aspect = Window::currentWindow->GetAspect();
    PerspectiveMatrix(m_CurrentCamera->fov * Deg2Rad, aspect,m_CurrentCamera->near,m_CurrentCamera->far,&cameraBuffer.proj);
    cameraBuffer.proj[1][1] *= -1;
    m_UniformBuffers[_currentFrame].Update(&cameraBuffer.view.data[0].x,sizeof(cameraBuffer));
    
}

void Renderer::UpdateLightBuffer(uint32_t _currentFrame)
{
    std::vector<DirLight>* dirlights = nullptr;
    std::vector<PointLight>* pointLights = nullptr;
    std::vector<SpotLight>* spotLights = nullptr;

    m_CurrentWorld->scene.GetComponentData<DirLight>(&dirlights);
    m_CurrentWorld->scene.GetComponentData<PointLight>(&pointLights);
    m_CurrentWorld->scene.GetComponentData<SpotLight>(&spotLights);
    
    // Set nbr
    uint32_t nbrOfDirLight = 0;
    uint32_t nbrOfPointLight = 0;
    uint32_t nbrOfSpotLight = 0;

    for (uint32_t i = 0; i < dirlights->size(); i++)
    {
        if (!IsValid(dirlights->at(i).componentHolder))
            continue;
        
        const Transform& transform = *m_CurrentWorld->scene.GetComponent<Transform>(dirlights->at(i).componentHolder.entityID);

        Vector3f dir = transform.localRotation * -Vector3f::UnitZ();
        
        m_GpuLights->gpuDirLights[i].direction = dir.Normalize();
        m_GpuLights->gpuDirLights[i].color = dirlights->at(i).color;
        m_GpuLights->gpuDirLights[i].intensity = dirlights->at(i).intensity;
        nbrOfDirLight++;
    }

    for (uint32_t i = 0; i < pointLights->size(); i++)
    {
        if (!IsValid(pointLights->at(i).componentHolder))
            continue;
        
        const Transform& transform = *m_CurrentWorld->scene.GetComponent<Transform>(dirlights->at(i).componentHolder.entityID);
        m_GpuLights->gpuPointLights[i].position = transform.position;
        m_GpuLights->gpuDirLights[i].color = pointLights->at(i).color;
        m_GpuLights->gpuDirLights[i].intensity = pointLights->at(i).intensity;
        nbrOfPointLight++;
    }

    for (uint32_t i = 0; i < spotLights->size(); i++)
    {
        if (!IsValid(spotLights->at(i).componentHolder))
            continue;
        
        const Transform& transform = *m_CurrentWorld->scene.GetComponent<Transform>(dirlights->at(i).componentHolder.entityID);
        Vector3f dir = transform.localRotation * -Vector3f::UnitZ();
        m_GpuLights->gpuSpotLight[i].position = transform.position;
        m_GpuLights->gpuSpotLight[i].direction = dir;
        m_GpuLights->gpuSpotLight[i].color = spotLights->at(i).color;
        m_GpuLights->gpuSpotLight[i].intensity = spotLights->at(i).intensity;
        nbrOfSpotLight++;
    }

    m_GpuLights->nbrOfDirLight = static_cast<int32_t>(nbrOfDirLight);
    m_GpuLights->nbrOfPointLight = static_cast<int32_t>(nbrOfPointLight);
    m_GpuLights->nbrOfSpotLight = static_cast<int32_t>(nbrOfSpotLight);
    
    m_ShaderStoragesLight[_currentFrame].Update(m_GpuLights,sizeof(GpuLight));
    
}

void Renderer::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = VulkanUniformBuffer::GetLayoutBinding(0,1 ,
        VK_SHADER_STAGE_VERTEX_BIT);
    VkDescriptorSetLayoutBinding modelMatriciesBufferStorage = VulkanShaderStorageBuffer::GetLayoutBinding(1,1, VK_SHADER_STAGE_VERTEX_BIT);
    VkDescriptorSetLayoutBinding shaderBufferStorageLight = VulkanShaderStorageBuffer::GetLayoutBinding(2,1, VK_SHADER_STAGE_FRAGMENT_BIT);

    m_DescriptorSetLayout.Init({uboLayoutBinding , modelMatriciesBufferStorage, shaderBufferStorageLight});
    m_DescriptorPool.Init(m_DescriptorSetLayout.GetLayoutBinding(),VulkanInterface::GetNbrOfImage());
}

void Renderer::CreateDescriptorSets()
{
    descriptorSets.resize(VulkanInterface::GetNbrOfImage());
    m_DescriptorPool.CreateDescriptorSet(m_DescriptorSetLayout.Get(), descriptorSets.size(),descriptorSets.data());
    
    for (size_t i = 0; i < descriptorSets.size(); i++)
    {
       
        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_UniformBuffers[i].GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(cameraBuffer);
        
        m_UniformBuffers[i].Bind(&descriptorWrites[0], descriptorSets[i],
            0, 0, 1,
            bufferInfo);

        VkDescriptorBufferInfo modelMatriciesInfo{};
        modelMatriciesInfo.buffer = m_ModelMatriciesShaderStorages[i].GetHandle();
        modelMatriciesInfo.offset = 0;
        modelMatriciesInfo.range = sizeof(MatrixMeshes) * m_MatrixMeshs.size();
        
        m_ModelMatriciesShaderStorages[i].Bind(descriptorWrites.data() + 1, descriptorSets[i], 1,0,1,
            modelMatriciesInfo);
        
        VkDescriptorBufferInfo shaderStoragebufferInfoLight{};
        shaderStoragebufferInfoLight.buffer = m_ShaderStoragesLight[i].GetHandle();
        shaderStoragebufferInfoLight.offset = 0;
        shaderStoragebufferInfoLight.range = sizeof(GpuLight);
        
        m_ShaderStoragesLight[i].Bind(descriptorWrites.data() + 2, descriptorSets[i], 2,0,1,
            shaderStoragebufferInfoLight);
        
       vkUpdateDescriptorSets(VulkanInterface::GetDevice().device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void Renderer::ComputeModelAndNormalInvertMatrix(uint32_t _currentFrame)
{
    const Scene& scene = m_CurrentWorld->scene;
    std::vector<Transform>* m_transforms = nullptr;
    scene.GetComponentData<Transform>(&m_transforms);
    
    for (int i = 0; i < m_transforms->size(); ++i)
    {
        const Transform& transform = m_transforms->at(i);

        if (!IsValid(transform.componentHolder))
            continue;
        
        MatrixMeshes& matricies = m_MatrixMeshs[transform.componentHolder.entityID];
        Trs3D(transform.localPosition,transform.localRotation.Normalize() ,transform.scale, &matricies.model);
        Matrix4x4f invertedModel;
        Invert<float>(matricies.model, &invertedModel);
        matricies.modelNormalMatrix = invertedModel.Transpose();
        
    }

    m_ModelMatriciesShaderStorages[_currentFrame].Update(m_MatrixMeshs.data(),sizeof(m_MatrixMeshs[0]) * m_MatrixMeshs.size());
}

void Renderer::DrawStatisMesh(VkCommandBuffer commandBuffer, uint32_t imageIndex, const StaticMesh& staticMesh,
    const Transform& transform, const Entity& entity)
{
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
    const VkBuffer vertexBuffers[] = {staticMesh.mesh->vulkanVertexBuffer.GetHandle()};
    const VkDeviceSize offsets[] = {0};

    
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipelineLayout.Get(), 0,
        1, &descriptorSets[VulkanInterface::GetCurrentFrame()], 0, nullptr);
    VulkanInterface::vulkanMaterialManager.BindMaterialDescriptorSet(commandBuffer, *staticMesh.material,m_VkPipelineLayout.Get());

    
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, staticMesh.mesh->vulkanIndexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdPushConstants(commandBuffer, m_VkPipelineLayout.Get(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(int32_t), &entity);
    vkCmdDrawIndexed(commandBuffer, staticMesh.mesh->indicies.size(), 1, 0, 0, 0);
}

void Renderer::InitBuffers()
{
    const size_t nbrOfImage = VulkanInterface::GetNbrOfImage();  
    
    m_UniformBuffers.resize(nbrOfImage);
    for(VulkanUniformBuffer& uniformBuffer : m_UniformBuffers)
    {
        uniformBuffer.Init(&cameraBuffer, sizeof(cameraBuffer));
    }
    
    m_ModelMatriciesShaderStorages.resize(nbrOfImage);
    for(VulkanShaderStorageBuffer& shaderStorageBuffer : m_ModelMatriciesShaderStorages)
    {
        shaderStorageBuffer.Init(m_MatrixMeshs.size() * sizeof(MatrixMeshes));
    }
    
    m_ShaderStoragesLight.resize(nbrOfImage);
    for(VulkanShaderStorageBuffer& shaderStorageBuffer : m_ShaderStoragesLight)
    {
        shaderStorageBuffer.Init(sizeof(GpuLight));
    }
}
