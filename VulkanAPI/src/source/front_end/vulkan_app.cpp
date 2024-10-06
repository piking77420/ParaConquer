#include "front_end/vulkan_app.hpp"


uint32_t VK_NP::VulkanApp::GetCurrentImage() const
{
    return m_VulkanContext.currentFrame;
}

void VK_NP::VulkanApp::BeginRender(PC_CORE::CommandPoolHandle _commandBuffer)
{
    m_BindCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);
    
    m_BindCommandBuffer.reset();

    
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    beginInfo.pInheritanceInfo = nullptr; // Optional

    VK_CALL(m_BindCommandBuffer.begin(&beginInfo));
    
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

void VK_NP::VulkanApp::EndRender()
{
    m_BindCommandBuffer.endRenderPass();
    m_BindCommandBuffer.end();
}

VK_NP::VulkanApp::VulkanApp(const VulkanAppCreateInfo& vulkanMainCreateInfo) :  m_vulkanHardwareWrapper(vulkanMainCreateInfo)
                                                                                , m_vulkanPresentChain(vulkanMainCreateInfo)
{
}

VK_NP::VulkanApp::~VulkanApp()
{
}

void VK_NP::VulkanApp::WaitForAquireImage()
{
    m_vulkanPresentChain.WaitForAvailableImage(&m_VulkanContext);
    m_vulkanPresentChain.AquireNetImageKHR(&m_VulkanContext);
}

void VK_NP::VulkanApp::SwapBuffers(PC_CORE::CommandBufferHandle* _commandBuffers, uint32_t _commandBufferCount)
{
     vk::CommandBuffer* vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer*>(_commandBuffers);

    m_vulkanPresentChain.SwapBuffer(vkCommandBuffer, _commandBufferCount, &m_VulkanContext);
    m_VulkanContext.currentFrame = (m_VulkanContext.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


 void VK_NP::VulkanApp::BindShaderProgram(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName)
{
     vk::CommandBuffer vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);

    m_vulkanShaderManager.BindProgram(vkCommandBuffer, _shaderProgramName);
}

void VK_NP::VulkanApp::PushConstants(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName,
            const std::string& _pushConstantName, const void* _data, uint32_t _size)
{
    vk::CommandBuffer vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);
  
    m_vulkanShaderManager.PushConstant(_shaderProgramName, _pushConstantName.c_str(), _data, _size , vkCommandBuffer);
}



void VK_NP::VulkanApp::RecreateSwapChain(void* _glfwWindowptr, uint32_t _newWidht, uint32_t _newHeight)
{
    WaitDevice();
    m_vulkanPresentChain.RecreateSwapChain(_glfwWindowptr, _newWidht, _newHeight);
}



void VK_NP::VulkanApp::CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourceAndPath>& _shaderSource)
{
    m_vulkanShaderManager.CreateShaderFromSource(programShaderCreateInfo, _shaderSource);
}

bool VK_NP::VulkanApp::DestroyShader(const std::string& _shaderProgramName)
{
    return m_vulkanShaderManager.DestroyShader(_shaderProgramName);
}

VULKAN_API void VK_NP::VulkanApp::WaitDevice()
{
    m_VulkanContext.device.waitIdle();
}

PC_CORE::GPUBufferHandle VK_NP::VulkanApp::BufferData(size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage)
{
   return bufferMap.CreateBuffer(static_cast<uint32_t>(_size), _data, _usage);
}

bool VK_NP::VulkanApp::DestroyBuffer(PC_CORE::GPUBufferHandle _handle)
{
    return bufferMap.DestroyBuffer(_handle);
}

void VK_NP::VulkanApp::BindBuffer(PC_CORE::CommandBufferHandle _commandBuffer, PC_CORE::GPUBufferHandle _handle)
{
    auto& bufferInfo = bufferMap.GetBufferUsage(_handle);
    vk::CommandBuffer vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);
    
    vk::Buffer vkBuffer[] =
        {
        CastObjectToVkObject<vk::Buffer>(_handle)
    };
    vk::DeviceSize offset[]= {0};
    
    switch (bufferInfo.usage)
    {
    case PC_CORE::GPU_BUFFER_USAGE::VERTEX:
        vkCommandBuffer.bindVertexBuffers(0, 1, vkBuffer, offset);
        break;
    case PC_CORE::GPU_BUFFER_USAGE::INDEX:
        break;
    case PC_CORE::GPU_BUFFER_USAGE::UNIFORM:
        break;
    case PC_CORE::GPU_BUFFER_USAGE::SHADER_STORAGE:
        break;
    case PC_CORE::GPU_BUFFER_USAGE::TRANSFERT:
        break;
    }
}

#pragma region CommandPool Functions

void VK_NP::VulkanApp:: CreateCommandPool(const PC_CORE::CommandPoolCreateInfo& _commandPoolCreateInfo,
    PC_CORE::CommandPoolHandle* _commandPoolHandle)
{
    vk::CommandPoolCreateInfo _vkcommandPoolCreateInfo;
    _vkcommandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    _vkcommandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    // TODO MAKE A WRAPPING FROM COMMAND POOL USAGE TO QUEUE FAMILILES
    _vkcommandPoolCreateInfo.queueFamilyIndex = 0;

    vk::CommandPool commandPool = VulkanContext::currentContext->device.createCommandPool(_vkcommandPoolCreateInfo, nullptr);
    *_commandPoolHandle = commandPool;
}

void VK_NP::VulkanApp::DestroyCommandPool(PC_CORE::CommandPoolHandle _commandPoolHandle)
{
    vk::CommandPool commandPool = CastObjectToVkObject<vk::CommandPool>(_commandPoolHandle);

    VulkanContext::currentContext->device.destroyCommandPool(commandPool);
}

void VK_NP::VulkanApp::AllocCommandBuffers(PC_CORE::CommandPoolHandle _commandPoolHandle, PC_CORE::CommandBufferHandle* _commandBuffer,
    uint32_t _commandBufferAllocationCount)
{
    vk::CommandPool commandPool = CastObjectToVkObject<vk::CommandPool>(_commandPoolHandle);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = _commandBufferAllocationCount;
    
    vk::CommandBuffer* vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer*>(_commandBuffer);
    VK_CALL(VulkanContext::currentContext->device.allocateCommandBuffers(&commandBufferAllocateInfo,
        vkCommandBuffer));
}

void VK_NP::VulkanApp::FreeCommandBuffers(PC_CORE::CommandPoolHandle _commandPoolHandle, const PC_CORE::CommandBufferHandle* _commandBuffer,
    uint32_t _commandBufferFreeAllocationCount)
{
    vk::CommandPool commandPool = CastObjectToVkObject<vk::CommandPool>(_commandPoolHandle);
    const vk::CommandBuffer* vkCommandBuffer = CastObjectToVkObjectConst<const vk::CommandBuffer*>(_commandBuffer);

    VulkanContext::currentContext->device.freeCommandBuffers(commandPool, _commandBufferFreeAllocationCount,
        vkCommandBuffer);
}

void VK_NP::VulkanApp::SetViewPort(PC_CORE::CommandBufferHandle _commandBufferHandle,
    const PC_CORE::ViewPort& _viewPort)
{
    vk::CommandBuffer commandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBufferHandle);
    
    vk::Viewport viewport{};
    viewport.x = _viewPort.position.x;
    viewport.y = _viewPort.position.y;
    viewport.width = static_cast<float>(_viewPort.width);
    viewport.height = static_cast<float>(_viewPort.height);
    viewport.minDepth = _viewPort.minDepth;
    viewport.maxDepth = _viewPort.maxDepth;
    commandBuffer.setViewport(0, 1, &viewport);

}

void VK_NP::VulkanApp::SetScissor(PC_CORE::CommandBufferHandle _commandBufferHandle,
    const PC_CORE::ScissorRect& _scissorRect)
{
    vk::CommandBuffer commandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBufferHandle);

    vk::Rect2D scissor{};
    scissor.offset.x = _scissorRect.offset.x;
    scissor.offset.x = _scissorRect.offset.x;

    scissor.extent.width = _scissorRect.extend.x;
    scissor.extent.height = _scissorRect.extend.y;

    commandBuffer.setScissor(0, 1, &scissor);
    
}

void VK_NP::VulkanApp::Draw(PC_CORE::CommandBufferHandle _commandBufferHandle, uint32_t _vertexCount,
    uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    vk::CommandBuffer commandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBufferHandle);
    commandBuffer.draw(_vertexCount, instanceCount, firstVertex, firstInstance);
}
#pragma endregion CommandPool Functions

