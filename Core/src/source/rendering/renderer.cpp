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
#include "time/timer.hpp"
#include <thread>

#include "world/transform.hpp"

using namespace PC_CORE;

void Renderer::Init(Window* _window)
{
    vulkanViewport.Init(this);
    const ShaderSource* vertex = ResourceManager::Get<ShaderSource>("shader_base.vert");
    const ShaderSource* frag = ResourceManager::Get<ShaderSource>("shader_base.frag");
    m_VulkanShaderStage.Init({vertex, frag});

    InitRenderPasses();
    m_CommandBuffers.resize(VulkanInterface::GetNbrOfImage());
    VulkanInterface::vulkanCommandPoolGraphic.AllocCommandBuffer(VulkanInterface::GetNbrOfImage(),
                                                                 m_CommandBuffers.data());
    const VkCommandPoolCreateInfo vkCommandPoolCreateInfoGraphic =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = VulkanInterface::vulkanDevice.graphicsQueue.index
    };
    fwdCommandPool.Init(&vkCommandPoolCreateInfoGraphic);
    m_ForwardCommandBuffers.resize(VulkanInterface::GetNbrOfImage());
    fwdCommandPool.AllocCommandBuffer(m_ForwardCommandBuffers.size(), m_ForwardCommandBuffers.data());

    CreateAsyncObject();
    InitBuffers();
    CreateDescriptorSetLayout();
    CreateDescriptorSets();
    CreateBasicGraphiPipeline();
    drawGizmos.Init(this);
    drawQuad.Init(this);
    skyboxRender.Init(this);
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
    
    vulkanViewport.Destroy();
    drawGizmos.Destroy();
    drawQuad.Destroy();
    skyboxRender.Destroy();

    for (VulkanUniformBuffer& uniformBuffer : m_CameraBuffers)
    {
        uniformBuffer.Destroy();
    }
    for (VulkanShaderStorageBuffer& vulkanShaderStorageBuffer : m_ModelMatriciesShaderStorages)
    {
        vulkanShaderStorageBuffer.Destroy();
    }
    for (VulkanShaderStorageBuffer& vulkanShaderStorageBuffer : m_ShaderStoragesLight)
    {
        vulkanShaderStorageBuffer.Destroy();
    }
    for (VulkanRenderPass& renderPass : renderPasses)
    {
        renderPass.Destroy();
    }
    m_DescriptorSetLayout.Destroy();
    m_DescriptorPool.Destroy();
    fwdCommandPool.Destroy();

    m_VulkanShaderStage.Destroy();
    m_VkPipelineLayout.Destroy();
    m_BasePipeline.Destroy();
    DestroyAsyncObject();
}

void Renderer::BeginFrame(const World& world)
{
    const VkDevice& device = VulkanInterface::GetDevice().device;
    const uint32_t currentFrame = VulkanInterface::GetCurrentFrame();

    VkResult result = vkWaitForFences(device, 1, &asyncObjet.m_InFlightFence[currentFrame].fences, VK_TRUE, UINT64_MAX);

    VK_CHECK_ERROR(result, "vkWaitForFences")
    
    result = vkAcquireNextImageKHR(device, VulkanInterface::vulkanSwapChapchain.swapchainKhr, UINT64_MAX,
                                   asyncObjet.m_ImageAvailableSemaphore[currentFrame].semaphore,
                                   VK_NULL_HANDLE, &m_ImageIndex);
    
    VK_CHECK_ERROR(result, "vkAcquireNextImageKHR")
    result = vkResetFences(device, 1, &asyncObjet.m_InFlightFence[currentFrame].fences);
    VK_CHECK_ERROR(result, "vkResetFences")
    
    result = vkResetCommandBuffer(m_CommandBuffers[currentFrame], 0);
    VK_CHECK_ERROR(result, "vkResetCommandBuffer m_CommandBuffers")
    
    result = vkResetCommandBuffer(m_ForwardCommandBuffers[currentFrame], 0);
    VK_CHECK_ERROR(result, "m_ForwardCommandBuffers[currentFrame]")
    
    m_CurrentWorld = &world;
    UpdateWorldBuffers();
}


void Renderer::RenderViewPort(const Camera& _camera, const uint32_t viewPortId,
                              const World& _world)
{
    m_CurrentCamera = &_camera;
    m_CurrentWorld = &_world;
    m_CurrentViewport = &vulkanViewport.GetViewPort(viewPortId);

    UpdateCameraBuffer(VulkanInterface::GetCurrentFrame());

    const uint32_t currentFrame = VulkanInterface::GetCurrentFrame();
    const VkCommandBuffer& vkCommandBuffer = m_ForwardCommandBuffers[VulkanInterface::GetCurrentFrame()];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginCommandBuffer(vkCommandBuffer, 0);


    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_CurrentViewport->viewPortSize.x);
    viewport.height = static_cast<float>(m_CurrentViewport->viewPortSize.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {
        static_cast<uint32_t>(m_CurrentViewport->viewPortSize.x),
        static_cast<uint32_t>(m_CurrentViewport->viewPortSize.y)
    };

    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);

    ForwardPass(vkCommandBuffer);

    const VkResult r = vkEndCommandBuffer(vkCommandBuffer);
    VK_CHECK_ERROR(r, "Failed to begin EndCommandBuffer")
}

void Renderer::SwapBuffers()
{
    RenderSwapChain();

    const uint32_t currentFrame = VulkanInterface::GetCurrentFrame();

    // Setup submit info for command buffer submission
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const VkSemaphore waitSemaphores[] = {asyncObjet.m_ImageAvailableSemaphore[currentFrame].semaphore};
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    std::vector<VkCommandBuffer> commandBuffers = {
        m_ForwardCommandBuffers[currentFrame], m_CommandBuffers[currentFrame]
    };
    submitInfo.commandBufferCount = commandBuffers.size();
    submitInfo.pCommandBuffers = commandBuffers.data();

    const VkSemaphore signalSemaphores[] = {asyncObjet.m_RenderFinishedSemaphore[currentFrame].semaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // Submit command buffer
    VkResult result = vkQueueSubmit(VulkanInterface::GetDevice().graphicsQueue.Queue, 1, &submitInfo,
                                    asyncObjet.m_InFlightFence[currentFrame].fences);

    VK_CHECK_ERROR(result, "failed to vkQueueSubmit");


    // Setup present info for presentation
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    const VkSwapchainKHR swapChains[] = {VulkanInterface::vulkanSwapChapchain.swapchainKhr};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_ImageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(VulkanInterface::GetDevice().graphicsQueue.Queue, &presentInfo);
    VK_CHECK_ERROR(result, "failed to vkQueuePresentKHR");

    VulkanInterface::ComputeNextFrame();
}

void Renderer::RenderSwapChain()
{
    const VkCommandBuffer& vkCommandBuffer = m_CommandBuffers[VulkanInterface::GetCurrentFrame()];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginCommandBuffer(vkCommandBuffer, 0);


    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = VulkanInterface::vulkanSwapChapchain.swapchainRenderPass.renderPass;
    renderPassInfo.framebuffer = VulkanInterface::GetSwapChainFramebuffer(m_ImageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VulkanInterface::vulkanSwapChapchain.swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(VulkanInterface::vulkanSwapChapchain.swapChainExtent.width);
    viewport.height = static_cast<float>(VulkanInterface::vulkanSwapChapchain.swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VulkanInterface::vulkanSwapChapchain.swapChainExtent;
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);

    // DrawSwapChain Image if real game

    VulkanImgui::Render(GetCurrentCommandBuffer());
    vkCmdEndRenderPass(vkCommandBuffer);

    const VkResult r = vkEndCommandBuffer(vkCommandBuffer);
    VK_CHECK_ERROR(r, "Failed to begin EndCommandBuffer")
}


void Renderer::InitRenderPasses()
{
    const Attachment color =
    {
        .attachementIndex = AttachementIndex::Color00,
        .format = VulkanInterface::vulkanSwapChapchain.surfaceFormatKhr.format,
        .clearOnLoad = true,
        .write = true,
        .imageLayoutInit = VK_IMAGE_LAYOUT_UNDEFINED,
        .imageLayoutRef = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .imageLayoutFinal = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    const Attachment depth =
    {
        .attachementIndex = AttachementIndex::Depth,
        .format = VulkanInterface::vulkanSwapChapchain.depthFormat,
        .clearOnLoad = true,
        .write = false,
        .imageLayoutInit = VK_IMAGE_LAYOUT_UNDEFINED,
        .imageLayoutRef = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .imageLayoutFinal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };
    
    std::vector<Attachment> attachmentsColor  =
    {
        color
    };
    std::vector<Attachment> attachmentsfwd  =
    {
        color,depth
    };
    
    renderPasses.at(COLORONLY).Init(attachmentsColor, 0);
    renderPasses.at(FORWARD).Init(attachmentsfwd, 0); //, depth});
    
}

void Renderer::BeginCommandBuffer(VkCommandBuffer _commandBuffer, VkCommandBufferUsageFlags _usageFlags)
{
    VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = _usageFlags,
        .pInheritanceInfo = nullptr
    };

    const VkResult r = vkBeginCommandBuffer(_commandBuffer, &vkCommandBufferBeginInfo);
    VK_CHECK_ERROR(r, "Failed to begin CommandBuffer")
}


void Renderer::ForwardPass(VkCommandBuffer commandBuffer)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPasses.at(FORWARD).renderPass;
    renderPassInfo.framebuffer = m_CurrentViewport->forwardAttachments.at(m_ImageIndex).framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {
        static_cast<uint32_t>(m_CurrentViewport->viewPortSize.x),
        static_cast<uint32_t>(m_CurrentViewport->viewPortSize.y)
    };


    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0, 0};
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_BasePipeline.Get());

    // TODO Update ECS
    /*
    const std::vector<StaticMesh>* meshes = nullptr;
    m_CurrentWorld->scene.GetComponentData<StaticMesh>(&meshes);

    for (size_t i = 0; i < meshes->size(); i++)
    {
        const StaticMesh& staticMesh = meshes->at(i);

        if (!IsValid(staticMesh.componentHolder))
            continue;

        const Entity& entity = staticMesh.componentHolder.entityID;
        const Transform& transform = *m_CurrentWorld->scene.GetComponent<Transform>(entity);
        DrawStatisMesh(commandBuffer, m_ImageIndex, staticMesh, transform, entity);
    }
    */
    drawGizmos.DrawGizmosForward(commandBuffer, m_ImageIndex, *m_CurrentViewport);
    skyboxRender.DrawSkybox(commandBuffer, m_CurrentWorld->skybox);
    vkCmdEndRenderPass(commandBuffer);
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

    VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = Vertex::GetAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.pNext = nullptr;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pDepthStencilState = &depthStencil;

    m_BasePipeline.Init(&pipelineInfo, m_VulkanShaderStage, m_VkPipelineLayout.Get(),
                        renderPasses.at(FORWARD).renderPass);
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
    asyncObjet.m_ImageAvailableSemaphore.resize(nbrOfImage);
    asyncObjet.m_RenderFinishedSemaphore.resize(nbrOfImage);
    asyncObjet.m_InFlightFence.resize(nbrOfImage);

    for (size_t i = 0; i < VulkanInterface::GetNbrOfImage(); i++)
    {
        asyncObjet.m_ImageAvailableSemaphore[i].Init(semaphoreInfo);
        asyncObjet.m_RenderFinishedSemaphore[i].Init(semaphoreInfo);
        asyncObjet.m_InFlightFence[i].Init(fenceInfo);
    }
}

void Renderer::DestroyAsyncObject()
{
    const size_t nbrOfImage = VulkanInterface::GetNbrOfImage();

    for (size_t i = 0; i < nbrOfImage; i++)
    {
        asyncObjet.m_ImageAvailableSemaphore[i].Destroy();
        asyncObjet.m_RenderFinishedSemaphore[i].Destroy();
        asyncObjet.m_InFlightFence[i].Destroy();
    }
}

void Renderer::UpdateCameraBuffer(uint32_t _currentFrame)
{
    cameraBuffer.cameraPos = m_CurrentCamera->position;

    Tbx::LookAtRH<float>(m_CurrentCamera->position, m_CurrentCamera->position + m_CurrentCamera->front, m_CurrentCamera->up,
             &cameraBuffer.view);
    const float aspect = Window::currentWindow->GetAspect();
    PerspectiveMatrix(m_CurrentCamera->fov * Deg2Rad, aspect, m_CurrentCamera->near, m_CurrentCamera->far,
                      &cameraBuffer.proj);
    m_CameraBuffers[_currentFrame].Update(&cameraBuffer.view[0][0], sizeof(cameraBuffer));
    
}

void Renderer::UpdateLightBuffer(uint32_t _currentFrame)
{
   BEGIN_TIMER("LightPassBuffer")

   const std::vector<DirLight>* dirlights = nullptr;
   const std::vector<PointLight>* pointLights = nullptr;
   const std::vector<SpotLight>* spotLights = nullptr;
    // TODO Update ECS
/*
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
        {
            m_GpuLights.gpuDirLights[i] = {};
            continue;
        }

        const Transform& transform = *m_CurrentWorld->scene.GetComponent<Transform>(
            dirlights->at(i).componentHolder.entityID);

        Tbx::Vector3f dir = transform.rotation * Tbx::Vector3f::UnitY();

        m_GpuLights.gpuDirLights[i].direction = dir.Normalize();
        m_GpuLights.gpuDirLights[i].color = dirlights->at(i).color;
        m_GpuLights.gpuDirLights[i].intensity = dirlights->at(i).intensity;
        nbrOfDirLight++;
    }

    for (uint32_t i = 0; i < pointLights->size(); i++)
    {
        if (!IsValid(pointLights->at(i).componentHolder))
        {
            m_GpuLights.gpuSpotLight[i] = {};
            continue;
        }

        const Transform& transform = *m_CurrentWorld->scene.GetComponent<Transform>(
            pointLights->at(i).componentHolder.entityID);
        m_GpuLights.gpuPointLights[i].position = transform.position;
        m_GpuLights.gpuDirLights[i].color = pointLights->at(i).color;
        m_GpuLights.gpuDirLights[i].intensity = pointLights->at(i).intensity;
        nbrOfPointLight++;
    }

    for (uint32_t i = 0; i < spotLights->size(); i++)
    {
        if (!IsValid(spotLights->at(i).componentHolder))
        {
            m_GpuLights.gpuSpotLight[i] = {};
            continue;
        }

        const Transform& transform = *m_CurrentWorld->scene.GetComponent<Transform>(
            dirlights->at(i).componentHolder.entityID);
        Tbx::Vector3f dir = transform.rotation * -Tbx::Vector3f::UnitY();
        m_GpuLights.gpuSpotLight[i].position = transform.position;
        m_GpuLights.gpuSpotLight[i].direction = dir;
        m_GpuLights.gpuSpotLight[i].color = spotLights->at(i).color;
        m_GpuLights.gpuSpotLight[i].intensity = spotLights->at(i).intensity;
        nbrOfSpotLight++;
    }

    m_GpuLights.nbrOfDirLight = static_cast<int32_t>(nbrOfDirLight);
    m_GpuLights.nbrOfPointLight = static_cast<int32_t>(nbrOfPointLight);
    m_GpuLights.nbrOfSpotLight = static_cast<int32_t>(nbrOfSpotLight);

    m_ShaderStoragesLight[_currentFrame].Update(&m_GpuLights, sizeof(GpuLight));*/
    END_TIMER()
}

void Renderer::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = VulkanUniformBuffer::GetLayoutBinding(0, 1,
        VK_SHADER_STAGE_VERTEX_BIT);
    VkDescriptorSetLayoutBinding modelMatriciesBufferStorage = VulkanShaderStorageBuffer::GetLayoutBinding(
        1, 1, VK_SHADER_STAGE_VERTEX_BIT);
    VkDescriptorSetLayoutBinding shaderBufferStorageLight = VulkanShaderStorageBuffer::GetLayoutBinding(
        2, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

    m_DescriptorSetLayout.Init({uboLayoutBinding, modelMatriciesBufferStorage, shaderBufferStorageLight});
    m_DescriptorPool.Init(m_DescriptorSetLayout.GetLayoutBinding(), VulkanInterface::GetNbrOfImage());
}

void Renderer::CreateDescriptorSets()
{
    descriptorSets.resize(VulkanInterface::GetNbrOfImage());
    m_DescriptorPool.CreateDescriptorSet(m_DescriptorSetLayout.Get(), descriptorSets.size(), descriptorSets.data());

    for (size_t i = 0; i < descriptorSets.size(); i++)
    {
        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_CameraBuffers[i].GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(cameraBuffer);

        m_CameraBuffers[i].Bind(&descriptorWrites[0], descriptorSets[i],
                                 0, 0, 1,
                                 bufferInfo);

        VkDescriptorBufferInfo modelMatriciesInfo{};
        modelMatriciesInfo.buffer = m_ModelMatriciesShaderStorages[i].GetHandle();
        modelMatriciesInfo.offset = 0;
        modelMatriciesInfo.range = SceneGraph::MatrixMeshesSize();

        m_ModelMatriciesShaderStorages[i].Bind(descriptorWrites.data() + 1, descriptorSets[i], 1, 0, 1,
                                               modelMatriciesInfo);

        VkDescriptorBufferInfo shaderStoragebufferInfoLight{};
        shaderStoragebufferInfoLight.buffer = m_ShaderStoragesLight[i].GetHandle();
        shaderStoragebufferInfoLight.offset = 0;
        shaderStoragebufferInfoLight.range = sizeof(GpuLight);

        m_ShaderStoragesLight[i].Bind(descriptorWrites.data() + 2, descriptorSets[i], 2, 0, 1,
                                      shaderStoragebufferInfoLight);

        vkUpdateDescriptorSets(VulkanInterface::GetDevice().device, static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}

void Renderer::ComputeModelAndNormalInvertMatrix(uint32_t _currentFrame)
{
    BEGIN_TIMER("Update Matricies ToGpu")
    const SceneGraph& sceneGraph = m_CurrentWorld->sceneGraph;
    
    m_ModelMatriciesShaderStorages[_currentFrame].Update(sceneGraph.globalMatricies.data(),
                                                         sizeof(sceneGraph.globalMatricies[0]) * sceneGraph.globalMatricies.size());
    END_TIMER()
}

// TODO
//
/*
void Renderer::DrawStatisMesh(VkCommandBuffer commandBuffer, uint32_t imageIndex, const StaticMesh& staticMesh,
                              const Transform& transform, const Entity& entity)
{
    const VkBuffer vertexBuffers[] = {staticMesh.mesh->vulkanVertexBuffer.GetHandle()};
    const VkDeviceSize offsets[] = {0};


    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkPipelineLayout.Get(), 0,
                            1, &descriptorSets[VulkanInterface::GetCurrentFrame()], 0, nullptr);
    VulkanInterface::vulkanMaterialManager.BindMaterialDescriptorSet(commandBuffer, 1, *staticMesh.material,
                                                                     m_VkPipelineLayout.Get());


    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, staticMesh.mesh->vulkanIndexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdPushConstants(commandBuffer, m_VkPipelineLayout.Get(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(int32_t),
                       &entity);
    vkCmdDrawIndexed(commandBuffer, staticMesh.mesh->indicies.size(), 1, 0, 0, 0);
}*/

void Renderer::UpdateWorldBuffers()
{
    const uint32_t currentFrame = VulkanInterface::GetCurrentFrame();
    ComputeModelAndNormalInvertMatrix(currentFrame);
    UpdateLightBuffer(currentFrame);
}

void Renderer::InitBuffers()
{
    const size_t nbrOfImage = VulkanInterface::GetNbrOfImage();

    m_CameraBuffers.resize(nbrOfImage);
    for (VulkanUniformBuffer& uniformBuffer : m_CameraBuffers)
    {
        uniformBuffer.Init(&cameraBuffer, sizeof(cameraBuffer));
    }

    m_ModelMatriciesShaderStorages.resize(nbrOfImage);
    for (VulkanShaderStorageBuffer& shaderStorageBuffer : m_ModelMatriciesShaderStorages)
    {
        shaderStorageBuffer.Init(SceneGraph::MatrixMeshesSize());
    }

    m_ShaderStoragesLight.resize(nbrOfImage);
    for (VulkanShaderStorageBuffer& shaderStorageBuffer : m_ShaderStoragesLight)
    {
        shaderStorageBuffer.Init(sizeof(GpuLight));
    }
}
