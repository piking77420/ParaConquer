#include "front_end/vulkan_app.hpp"

#include "back_end/rhi_vulkan_descriptorWrite.hpp"
#include "back_end/rhi_vulkan_descriptor_layout.hpp"
#include "back_end/rhi_vulkan_descriptor_pool.hpp"


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


VK_NP::VulkanApp::VulkanApp(const VulkanAppCreateInfo& vulkanMainCreateInfo)
{
    m_LogCallback = vulkanMainCreateInfo.logCallback;
    m_vulkanHardwareWrapper.Init(vulkanMainCreateInfo, &m_VulkanContext);
    m_DeleteFunction.push(std::bind(&VulkanHarwareWrapper::Destroy, &m_vulkanHardwareWrapper, std::placeholders::_1));
    m_vulkanPresentChain.Init(vulkanMainCreateInfo, &m_VulkanContext);
    m_DeleteFunction.push(std::bind(&VulkanPresentChain::Destroy, &m_vulkanPresentChain, std::placeholders::_1));
    m_vulkanShaderManager.Init(&m_VulkanContext);
    m_DeleteFunction.push(std::bind(&VulkanShaderManager::Destroy, &m_vulkanShaderManager, std::placeholders::_1));
    m_DeleteFunction.push(std::bind(&VulkanBufferMap::Destroy, &bufferMap, std::placeholders::_1));

    InitBaseObject();
}

VK_NP::VulkanApp::~VulkanApp()
{
    DestroyBaseObject();
    while (!m_DeleteFunction.empty())
    {
        m_DeleteFunction.top()(&m_VulkanContext);
        m_DeleteFunction.pop();
    }
}

ImGui_ImplVulkan_InitInfo VK_NP::VulkanApp::GetImGuiInitInfo()
{
    VulkanApp& singleton = dynamic_cast<VulkanApp&>(RHI::GetInstance());

    // https://github.com/ocornut/imgui/issues/5085
    VkDescriptorPoolSize pool_sizes[] =
    {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
    };

    vk::DescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = reinterpret_cast<vk::DescriptorPoolSize*>(&pool_sizes);

    vk::DescriptorPool descriptorPool = VK_NULL_HANDLE;
    descriptorPool = singleton.m_VulkanContext.device.createDescriptorPool(pool_info);

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = singleton.m_VulkanContext.instance;
    initInfo.PhysicalDevice = singleton.m_VulkanContext.physicalDevice;
    initInfo.Device = singleton.m_VulkanContext.device;
    initInfo.QueueFamily = singleton.m_VulkanContext.queuFamiliesIndicies.graphicsFamily;
    initInfo.Queue = singleton.m_VulkanContext.vkQueues.graphicQueue;
    initInfo.PipelineCache = nullptr;
    initInfo.DescriptorPool = descriptorPool;
    initInfo.RenderPass = singleton.m_VulkanContext.swapChainRenderPass;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = singleton.m_VulkanContext.swapChainImageCount - 1;
    initInfo.ImageCount = singleton.m_VulkanContext.swapChainImageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;

    return initInfo;
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


void VK_NP::VulkanApp::BindShaderProgram(PC_CORE::CommandBufferHandle _commandBuffer,
                                         const std::string& _shaderProgramName)
{
    vk::CommandBuffer vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);

    m_vulkanShaderManager.BindProgram(vkCommandBuffer, _shaderProgramName);
}

void VK_NP::VulkanApp::PushConstants(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName,
                                     const std::string& _pushConstantName, const void* _data, uint32_t _size)
{
    vk::CommandBuffer vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);

    m_vulkanShaderManager.PushConstant(_shaderProgramName, _pushConstantName.c_str(), _data, _size, vkCommandBuffer);
}


void VK_NP::VulkanApp::RecreateSwapChain(void* _glfwWindowptr, uint32_t _newWidht, uint32_t _newHeight)
{
    WaitDevice();
    m_VulkanContext.swapChainSupportDetails = VulkanPhysicalDevices::QuerySwapChainSupport(
        m_VulkanContext.physicalDevice, m_VulkanContext.surface);
    m_vulkanPresentChain.RecreateSwapChain(&m_VulkanContext, _glfwWindowptr, _newWidht, _newHeight);
}


void VK_NP::VulkanApp::CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo,
                                    const std::vector<PC_CORE::ShaderSourcePath>& _shaderSource)
{
    m_vulkanShaderManager.CreateShaderFromSource(m_VulkanContext.device, m_VulkanContext.swapChainRenderPass,
                                                 programShaderCreateInfo, _shaderSource);
}

bool VK_NP::VulkanApp::DestroyShader(const std::string& _shaderProgramName)
{
    return m_vulkanShaderManager.DestroyShader(m_VulkanContext.device, _shaderProgramName);
}


VULKAN_API void VK_NP::VulkanApp::WaitDevice()
{
    m_VulkanContext.device.waitIdle();
}

PC_CORE::GPUBufferHandle VK_NP::VulkanApp::BufferData(size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage)
{
    return bufferMap.CreateBuffer(&m_VulkanContext, m_VulkanContext.m_resourceCommandPool, static_cast<uint32_t>(_size),
                                  _data, _usage);
}

PC_CORE::GPUBufferHandle VK_NP::VulkanApp::BufferData(size_t _size, PC_CORE::GPU_BUFFER_USAGE _usage)
{
    return bufferMap.CreateBuffer(&m_VulkanContext, m_VulkanContext.m_resourceCommandPool, static_cast<uint32_t>(_size),
                                  nullptr, _usage);
}

void VK_NP::VulkanApp::MapData(PC_CORE::GPUBufferHandle _gpuBufferHandle, void** _data)
{
    bufferMap.MapData(&m_VulkanContext, _gpuBufferHandle, _data);
}

void VK_NP::VulkanApp::UnMapData(PC_CORE::GPUBufferHandle _gpuBufferHandle)
{
    bufferMap.UnMapData(&m_VulkanContext, _gpuBufferHandle);
}

bool VK_NP::VulkanApp::DestroyBuffer(PC_CORE::GPUBufferHandle _handle)
{
    return bufferMap.DestroyBuffer(&m_VulkanContext, _handle);
}

void VK_NP::VulkanApp::BindVertexBuffer(PC_CORE::CommandBufferHandle _commandBuffer, uint32_t _firstBinding,
                                        uint32_t _bindingCount, PC_CORE::GPUBufferHandle _handle)
{
    vk::CommandBuffer vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);

    vk::Buffer vkBuffer[] =
    {
        CastObjectToVkObject<vk::Buffer>(_handle)
    };
    vk::DeviceSize deviceOffSet[] = {0};

    vkCommandBuffer.bindVertexBuffers(0, 1, vkBuffer, deviceOffSet);
}

void VK_NP::VulkanApp::BindIndexBuffer(PC_CORE::CommandBufferHandle _commandBuffer, PC_CORE::GPUBufferHandle _handle)
{
    vk::CommandBuffer vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);

    vk::DeviceSize deviceOffSet[] = {0};

    vkCommandBuffer.bindIndexBuffer(CastObjectToVkObject<vk::Buffer>(_handle), deviceOffSet[0], vk::IndexType::eUint32);
}


void VK_NP::VulkanApp::CreateCommandPool(const PC_CORE::CommandPoolCreateInfo& _commandPoolCreateInfo,
                                         PC_CORE::CommandPoolHandle* _commandPoolHandle)
{
    vk::CommandPoolCreateInfo createInfo{};
    createInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    createInfo.flags = CommandPoolCreateFlagBitsToVulkan(_commandPoolCreateInfo.commandPoolBufferFlag);
    createInfo.queueFamilyIndex = GetQueueFamiliesIndexFromType(&m_VulkanContext, _commandPoolCreateInfo.queueType);

    vk::CommandPool* vkCommandPool = CastObjectToVkObject<vk::CommandPool*>(_commandPoolHandle);
    *vkCommandPool = m_VulkanContext.device.createCommandPool(createInfo, nullptr);
}

void VK_NP::VulkanApp::DestroyCommandPool(PC_CORE::CommandPoolHandle _commandPoolHandle)
{
    vk::CommandPool vkCommandPool = CastObjectToVkObject<vk::CommandPool>(_commandPoolHandle);
    m_VulkanContext.device.destroyCommandPool(vkCommandPool);
}

void VK_NP::VulkanApp::AllocCommandBuffers(PC_CORE::CommandPoolHandle _commandPoolHandle,
                                           PC_CORE::CommandBufferCreateInfo _commandBufferCreateInfo)
{
    vk::CommandPool commandPool = CastObjectToVkObject<vk::CommandPool>(_commandPoolHandle);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = commandPool;
    //TODO ACTTEPR ENUM FOR THI
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = _commandBufferCreateInfo.commandBufferCount;


    vk::CommandBuffer* vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer*>(
        _commandBufferCreateInfo.commandBufferPtr);
    VK_CALL(m_VulkanContext.device.allocateCommandBuffers(&commandBufferAllocateInfo,
        vkCommandBuffer));
}

void VK_NP::VulkanApp::FreeCommandBuffer(PC_CORE::CommandPoolHandle _commandPoolHandle,
                                         PC_CORE::CommandBuffer* _commandBuffer, uint32_t _commandBufferCount)
{
    vk::CommandPool commandPool = CastObjectToVkObject<vk::CommandPool>(_commandPoolHandle);
    const vk::CommandBuffer* vkCommandBuffer = CastObjectToVkObject<const vk::CommandBuffer*>(_commandBuffer);

    m_VulkanContext.device.freeCommandBuffers(commandPool, _commandBufferCount,
                                              vkCommandBuffer);
}


#pragma region CommandBuffer Functions

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
    scissor.offset.y = _scissorRect.offset.y;

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

void VK_NP::VulkanApp::DrawIndexed(PC_CORE::CommandBufferHandle _commandBufferHandle, uint32_t _indiciesCount,
                                   uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffSet,
                                   uint32_t firstInstance)
{
    vk::CommandBuffer commandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBufferHandle);
    commandBuffer.drawIndexed(_indiciesCount, instanceCount, firstIndex, vertexOffSet, firstInstance);
}


#pragma region DescriptorSetLayout
PC_CORE::DescriptorSetLayoutHandle VK_NP::VulkanApp::CreateDescriptorSetLayout(
    const std::vector<PC_CORE::DescriptorLayoutBinding>& _descriptorSetLayouts)
{
    vk::DescriptorSetLayout descriptorSetLayout = VK_NP::Backend::CreateDescriptorSetLayout(
        m_VulkanContext.device, _descriptorSetLayouts);

    return *reinterpret_cast<PC_CORE::DescriptorSetLayoutHandle*>(&descriptorSetLayout);
}

void VK_NP::VulkanApp::DestroyDescriptorSetLayout(const PC_CORE::DescriptorSetLayoutHandle& _descriptorSetLayoutHandle)
{
    Backend::DestroyDescriptorSetLayout(m_VulkanContext.device,
                                        CastObjectToVkObject<vk::DescriptorSetLayout>(_descriptorSetLayoutHandle));
}

PC_CORE::DescriptorPoolHandle VK_NP::VulkanApp::CreateDescriptorPool(
    const PC_CORE::DescriptorPoolSize* desciptorPoolSize, uint32_t descriptorCount,
    uint32_t maxSets)
{
    vk::DescriptorPool descriptorPool = Backend::RhiToVkDescriptorPool(m_VulkanContext.device, desciptorPoolSize,
                                                                       descriptorCount, maxSets);
    return *reinterpret_cast<PC_CORE::DescriptorPoolHandle*>(&descriptorPool);
}

void VK_NP::VulkanApp::DestroyDescriptorPool(PC_CORE::DescriptorPoolHandle _descriptorPoolHandle)
{
    vk::DescriptorPool descriptorPool = CastObjectToVkObject<vk::DescriptorPool>(_descriptorPoolHandle);
    Backend::DestroyDescriptorPool(m_VulkanContext.device, descriptorPool);
}

void VK_NP::VulkanApp::AllocDescriptorSet(PC_CORE::DescriptorSet* descriptorSets, uint32_t _descriptorSetCount,
                                          PC_CORE::DescriptorPoolHandle _descriptorPoolHandle,
                                          PC_CORE::DescriptorSetLayoutHandle _descriptorSetLayoutHandle)
{
    std::vector<vk::DescriptorSetLayout> layouts(_descriptorSetCount,
                                                 CastObjectToVkObject<vk::DescriptorSetLayout>(
                                                     _descriptorSetLayoutHandle));

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = CastObjectToVkObject<vk::DescriptorPool>(_descriptorPoolHandle);
    descriptorSetAllocateInfo.pSetLayouts = layouts.data();
    descriptorSetAllocateInfo.descriptorSetCount = _descriptorSetCount;


    // DescriptorSet shouldbe 8 byte long

    VK_CALL(
        m_VulkanContext.device.allocateDescriptorSets(&descriptorSetAllocateInfo, CastObjectToVkObject<vk::DescriptorSet
            *>(descriptorSets)));
}

void VK_NP::VulkanApp::UpdateDescriptorSet(uint32_t _descriptorWriteCount,
                                           PC_CORE::DescriptorWriteSet* _descriptorWrite)
{
    vk::DescriptorBufferInfo descriptorBufferInfo{};
    vk::DescriptorImageInfo descriptorImageInfo{};
    vk::BufferView bufferView{};

    const vk::WriteDescriptorSet vkDescriptorWrite = Backend::RhiToVulkanWriteDescriptorSet(
        *_descriptorWrite, &descriptorBufferInfo, &descriptorImageInfo, &bufferView);
    m_VulkanContext.device.updateDescriptorSets(_descriptorWriteCount, &vkDescriptorWrite, 0, nullptr);
}

void VK_NP::VulkanApp::BindDescriptorSet(PC_CORE::CommandBufferHandle _commandBuffer,
                                         const std::string& _shaderProgramName, uint32_t _firstSet,
                                         uint32_t _descriptorSetCount,
                                         const PC_CORE::DescriptorSet* _pDescriptorSets, uint32_t _dynamicOffsetCount,
                                         const uint32_t* _pDynamicOffsets)
{
    vk::CommandBuffer commandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);
    const vk::DescriptorSet* vkDescriptorSet = CastObjectToVkObject<const vk::DescriptorSet*>(_pDescriptorSets);

    auto shaderInternal = m_vulkanShaderManager.GetShader(_shaderProgramName);
    commandBuffer.bindDescriptorSets(shaderInternal.pipelineBindPoint, shaderInternal.pipelineLayout, _firstSet,
                                     _descriptorSetCount, vkDescriptorSet, _dynamicOffsetCount, _pDynamicOffsets);
}

#pragma endregion DescriptorSetLayout


#pragma endregion CommandBuffer Functions

#pragma region CommandPool Functions


#pragma endregion CommandPool Functions

#pragma region InitBaseObject

void VK_NP::VulkanApp::InitBaseObject()
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    commandPoolCreateInfo.queueFamilyIndex = m_VulkanContext.queuFamiliesIndicies.graphicsFamily;
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;

    m_VulkanContext.m_resourceCommandPool = m_VulkanContext.device.createCommandPool(commandPoolCreateInfo, nullptr);
}

void VK_NP::VulkanApp::DestroyBaseObject()
{
    m_VulkanContext.device.destroyCommandPool(m_VulkanContext.m_resourceCommandPool);
}
#pragma endregion InitBaseObject
