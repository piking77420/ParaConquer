#include "front_end/vulkan_app.hpp"

#include "rhi_vulkan_parser.hpp"
#include "back_end/rhi_vulkan_descriptor_write.hpp"
#include "back_end/rhi_vulkan_descriptor_layout.hpp"
#include "back_end/rhi_vulkan_descriptor_pool.hpp"
#include "back_end/vulkan_buffer.hpp"
#include "back_end/vulkan_command_pool_function.hpp"
#include "back_end/vulkan_image.hpp"


uint32_t Vulkan::VulkanApp::GetCurrentImage() const
{
    return m_VulkanContext.currentFrame;
}

void Vulkan::VulkanApp::BeginRender(PC_CORE::CommandPoolHandle _commandBuffer)
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

void Vulkan::VulkanApp::EndRender()
{
    m_BindCommandBuffer.endRenderPass();
    m_BindCommandBuffer.end();
}


Vulkan::VulkanApp::VulkanApp(const VulkanAppCreateInfo& vulkanMainCreateInfo)
{
    m_LogCallback = vulkanMainCreateInfo.logCallback;
    m_vulkanHardwareWrapper.Init(vulkanMainCreateInfo, &m_VulkanContext);
    m_DeleteFunction.push(std::bind(&VulkanHarwareWrapper::Destroy, &m_vulkanHardwareWrapper, std::placeholders::_1));
    m_vulkanPresentChain.Init(vulkanMainCreateInfo, &m_VulkanContext);
    m_DeleteFunction.push(std::bind(&VulkanPresentChain::Destroy, &m_vulkanPresentChain, std::placeholders::_1));
    m_vulkanShaderManager.Init(&m_VulkanContext);
    m_DeleteFunction.push(std::bind(&VulkanShaderManager::Destroy, &m_vulkanShaderManager, std::placeholders::_1));
    InitBaseObject();


    m_DeleteFunction.push(std::bind(&Vulkan::VulkanApp::DestroyBuffersAllocations, this, std::placeholders::_1));
    m_DeleteFunction.push(std::bind(&Vulkan::VulkanApp::DestroyImagesAllocations, this, std::placeholders::_1));
}

Vulkan::VulkanApp::~VulkanApp()
{
    DestroyBaseObject();
    while (!m_DeleteFunction.empty())
    {
        m_DeleteFunction.top()(&m_VulkanContext);
        m_DeleteFunction.pop();
    }
}

ImGui_ImplVulkan_InitInfo Vulkan::VulkanApp::GetImGuiInitInfo(PC_CORE::DescriptorPoolHandle* _descriptorPoolHandle)
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

    vk::DescriptorPool* vkDescriptorPool = CastObjectToVkObject<vk::DescriptorPool*>(_descriptorPoolHandle);

    *vkDescriptorPool = singleton.m_VulkanContext.device.createDescriptorPool(pool_info);
    

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = singleton.m_VulkanContext.instance;
    initInfo.PhysicalDevice = singleton.m_VulkanContext.physicalDevice;
    initInfo.Device = singleton.m_VulkanContext.device;
    initInfo.QueueFamily = singleton.m_VulkanContext.queuFamiliesIndicies.graphicsFamily;
    initInfo.Queue = singleton.m_VulkanContext.vkQueues.graphicQueue;
    initInfo.PipelineCache = nullptr;
    initInfo.DescriptorPool = *vkDescriptorPool;
    initInfo.RenderPass = singleton.m_VulkanContext.swapChainRenderPass;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = singleton.m_VulkanContext.swapChainImageCount - 1;
    initInfo.ImageCount = singleton.m_VulkanContext.swapChainImageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;

    return initInfo;
}


void Vulkan::VulkanApp::WaitForAquireImage()
{
    m_vulkanPresentChain.WaitForAvailableImage(&m_VulkanContext);
    m_vulkanPresentChain.AquireNetImageKHR(&m_VulkanContext);
}

void Vulkan::VulkanApp::SwapBuffers(PC_CORE::CommandBufferHandle* _commandBuffers, uint32_t _commandBufferCount)
{
    vk::CommandBuffer* vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer*>(_commandBuffers);

    m_vulkanPresentChain.SwapBuffer(vkCommandBuffer, _commandBufferCount, &m_VulkanContext);
    m_VulkanContext.currentFrame = (m_VulkanContext.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


void Vulkan::VulkanApp::BindShaderProgram(PC_CORE::CommandBufferHandle _commandBuffer,
                                          const std::string& _shaderProgramName)
{
    vk::CommandBuffer vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);

    m_vulkanShaderManager.BindProgram(vkCommandBuffer, _shaderProgramName);
}

void Vulkan::VulkanApp::PushConstants(PC_CORE::CommandBufferHandle _commandBuffer,
                                      const std::string& _shaderProgramName,
                                      const std::string& _pushConstantName, const void* _data, uint32_t _size)
{
    vk::CommandBuffer vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);

    m_vulkanShaderManager.PushConstant(_shaderProgramName, _pushConstantName.c_str(), _data, _size, vkCommandBuffer);
}


void Vulkan::VulkanApp::RecreateSwapChain(void* _glfwWindowptr, uint32_t _newWidht, uint32_t _newHeight)
{
    WaitDevice();
    m_VulkanContext.swapChainSupportDetails = VulkanPhysicalDevices::QuerySwapChainSupport(
        m_VulkanContext.physicalDevice, m_VulkanContext.surface);
    m_vulkanPresentChain.RecreateSwapChain(&m_VulkanContext, _glfwWindowptr, _newWidht, _newHeight);
}


void Vulkan::VulkanApp::CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo,
                                     const std::vector<PC_CORE::ShaderSourcePath>& _shaderSource)
{
    m_vulkanShaderManager.CreateShaderFromSource(m_VulkanContext.device, m_VulkanContext.swapChainRenderPass,
                                                 programShaderCreateInfo, _shaderSource);
}

bool Vulkan::VulkanApp::DestroyShader(const std::string& _shaderProgramName)
{
    return m_vulkanShaderManager.DestroyShader(m_VulkanContext.device, _shaderProgramName);
}

PC_CORE::DescriptorSetHandle* Vulkan::VulkanApp::GetDescriptorSet(const std::string& _shaderName)
{
    return reinterpret_cast<PC_CORE::DescriptorSetHandle*>(m_vulkanShaderManager.GetShaderInternal(_shaderName)->descriptorsets.data());
}


VULKAN_API void Vulkan::VulkanApp::WaitDevice()
{
    m_VulkanContext.device.waitIdle();
}

PC_CORE::GPUBufferHandle Vulkan::VulkanApp::BufferData(size_t _size, const void* _data,
                                                       PC_CORE::GPU_BUFFER_USAGE _usage)
{
    vk::Buffer buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VmaAllocationInfo allocationInfo = {};
    const uint32_t size = static_cast<uint32_t>(_size);
    vk::BufferUsageFlags bufferUsageFlags = GetVulkanUsage(_usage);


    if (bufferUsageFlags & vk::BufferUsageFlagBits::eVertexBuffer || bufferUsageFlags &
        vk::BufferUsageFlagBits::eIndexBuffer)
    {
        Backend::CreateGPUBufferFromCPU(&m_VulkanContext, m_VulkanContext.resourceCommandPool, size, _data, _usage,
                                        &buffer, &allocation);
    }
    else if (bufferUsageFlags & vk::BufferUsageFlagBits::eUniformBuffer || bufferUsageFlags &
        vk::BufferUsageFlagBits::eTransferSrc)
    {
        Backend::CreateBufferAndAlloc(&m_VulkanContext, size, static_cast<VkBufferUsageFlags>(bufferUsageFlags),
                                      VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
                                      VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
                                      &buffer, &allocation, &allocationInfo);
    }
    m_VulkanContext.m_BuffersAllocationMap.insert({VulkanObjectWrapper<vk::Buffer>(buffer), allocation});

    return *reinterpret_cast<PC_CORE::GPUBufferHandle*>(&buffer);
}

PC_CORE::GPUBufferHandle Vulkan::VulkanApp::BufferData(size_t _size, PC_CORE::GPU_BUFFER_USAGE _usage)
{
    return BufferData(_size, nullptr, _usage);
}

void Vulkan::VulkanApp::MapData(PC_CORE::GPUBufferHandle _gpuBufferHandle, void** _data)
{
    vk::Buffer vulkanBuffer = CastObjectToVkObject<vk::Buffer>(_gpuBufferHandle);
    if (!m_VulkanContext.m_BuffersAllocationMap.contains(VulkanObjectWrapper<vk::Buffer>(vulkanBuffer)))
    {
        return;
    }
    auto& it = m_VulkanContext.m_BuffersAllocationMap.at(VulkanObjectWrapper<vk::Buffer>(vulkanBuffer));
    vmaMapMemory(m_VulkanContext.allocator, it, _data);
}

void Vulkan::VulkanApp::UnMapData(PC_CORE::GPUBufferHandle _gpuBufferHandle)
{
    vk::Buffer vulkanBuffer = CastObjectToVkObject<vk::Buffer>(_gpuBufferHandle);
    if (!m_VulkanContext.m_BuffersAllocationMap.contains(VulkanObjectWrapper<vk::Buffer>(vulkanBuffer)))
    {
        return;
    }
    auto& it = m_VulkanContext.m_BuffersAllocationMap.at(VulkanObjectWrapper<vk::Buffer>(vulkanBuffer));
    vmaUnmapMemory(m_VulkanContext.allocator, it);
}

bool Vulkan::VulkanApp::DestroyBuffer(PC_CORE::GPUBufferHandle _handle)
{
    vk::Buffer vulkanBuffer = CastObjectToVkObject<vk::Buffer>(_handle);
    const VulkanObjectWrapper<vk::Buffer> bkBufferAsObject = VulkanObjectWrapper<vk::Buffer>(vulkanBuffer);

    if (!m_VulkanContext.m_BuffersAllocationMap.contains(VulkanObjectWrapper<vk::Buffer>(vulkanBuffer)))
    {
        return false;
    }
    auto& it = m_VulkanContext.m_BuffersAllocationMap.at(bkBufferAsObject);

    vmaFreeMemory(m_VulkanContext.allocator, it);
    m_VulkanContext.device.destroyBuffer(bkBufferAsObject.object);
    m_VulkanContext.m_BuffersAllocationMap.erase(bkBufferAsObject);
    return true;
}

void Vulkan::VulkanApp::BindVertexBuffer(PC_CORE::CommandBufferHandle _commandBuffer, uint32_t _firstBinding,
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

void Vulkan::VulkanApp::BindIndexBuffer(PC_CORE::CommandBufferHandle _commandBuffer, PC_CORE::GPUBufferHandle _handle)
{
    vk::CommandBuffer vkCommandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBuffer);

    vk::DeviceSize deviceOffSet[] = {0};

    vkCommandBuffer.bindIndexBuffer(CastObjectToVkObject<vk::Buffer>(_handle), deviceOffSet[0], vk::IndexType::eUint32);
}


void Vulkan::VulkanApp::CreateCommandPool(const PC_CORE::CommandPoolCreateInfo& _commandPoolCreateInfo,
                                          PC_CORE::CommandPoolHandle* _commandPoolHandle)
{
    vk::CommandPoolCreateInfo createInfo{};
    createInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    createInfo.flags = CommandPoolCreateFlagBitsToVulkan(_commandPoolCreateInfo.commandPoolBufferFlag);
    createInfo.queueFamilyIndex = GetQueueFamiliesIndexFromType(&m_VulkanContext, _commandPoolCreateInfo.queueType);

    vk::CommandPool* vkCommandPool = CastObjectToVkObject<vk::CommandPool*>(_commandPoolHandle);
    *vkCommandPool = m_VulkanContext.device.createCommandPool(createInfo, nullptr);
}

void Vulkan::VulkanApp::DestroyCommandPool(PC_CORE::CommandPoolHandle _commandPoolHandle)
{
    vk::CommandPool vkCommandPool = CastObjectToVkObject<vk::CommandPool>(_commandPoolHandle);
    m_VulkanContext.device.destroyCommandPool(vkCommandPool);
}

void Vulkan::VulkanApp::AllocCommandBuffers(PC_CORE::CommandPoolHandle _commandPoolHandle,
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

void Vulkan::VulkanApp::FreeCommandBuffer(PC_CORE::CommandPoolHandle _commandPoolHandle,
                                          PC_CORE::CommandBuffer* _commandBuffer, uint32_t _commandBufferCount)
{
    vk::CommandPool commandPool = CastObjectToVkObject<vk::CommandPool>(_commandPoolHandle);
    const vk::CommandBuffer* vkCommandBuffer = CastObjectToVkObject<const vk::CommandBuffer*>(_commandBuffer);

    m_VulkanContext.device.freeCommandBuffers(commandPool, _commandBufferCount,
                                              vkCommandBuffer);
}


#pragma region CommandBuffer Functions

void Vulkan::VulkanApp::SetViewPort(PC_CORE::CommandBufferHandle _commandBufferHandle,
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

void Vulkan::VulkanApp::SetScissor(PC_CORE::CommandBufferHandle _commandBufferHandle,
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

void Vulkan::VulkanApp::Draw(PC_CORE::CommandBufferHandle _commandBufferHandle, uint32_t _vertexCount,
                             uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    vk::CommandBuffer commandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBufferHandle);
    commandBuffer.draw(_vertexCount, instanceCount, firstVertex, firstInstance);
}

void Vulkan::VulkanApp::DrawIndexed(PC_CORE::CommandBufferHandle _commandBufferHandle, uint32_t _indiciesCount,
                                    uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffSet,
                                    uint32_t firstInstance)
{
    vk::CommandBuffer commandBuffer = CastObjectToVkObject<vk::CommandBuffer>(_commandBufferHandle);
    commandBuffer.drawIndexed(_indiciesCount, instanceCount, firstIndex, vertexOffSet, firstInstance);
}


#pragma region DescriptorSetLayout
PC_CORE::DescriptorSetLayoutHandle Vulkan::VulkanApp::CreateDescriptorSetLayout(
    const std::vector<PC_CORE::DescriptorLayoutBinding>& _descriptorSetLayouts)
{
    vk::DescriptorSetLayout descriptorSetLayout = Vulkan::Backend::CreateDescriptorSetLayout(
        m_VulkanContext.device, _descriptorSetLayouts);

    return *reinterpret_cast<PC_CORE::DescriptorSetLayoutHandle*>(&descriptorSetLayout);
}

void Vulkan::VulkanApp::DestroyDescriptorSetLayout(const PC_CORE::DescriptorSetLayoutHandle& _descriptorSetLayoutHandle)
{
    Backend::DestroyDescriptorSetLayout(m_VulkanContext.device,
                                        CastObjectToVkObject<vk::DescriptorSetLayout>(_descriptorSetLayoutHandle));
}

PC_CORE::DescriptorPoolHandle Vulkan::VulkanApp::CreateDescriptorPool(
    const PC_CORE::DescriptorPoolSize* desciptorPoolSize, uint32_t descriptorCount,
    uint32_t maxSets)
{
    vk::DescriptorPool descriptorPool = Backend::RhiToVkDescriptorPool(m_VulkanContext.device, desciptorPoolSize,
                                                                       descriptorCount, maxSets);
    return *reinterpret_cast<PC_CORE::DescriptorPoolHandle*>(&descriptorPool);
}

void Vulkan::VulkanApp::DestroyDescriptorPool(PC_CORE::DescriptorPoolHandle _descriptorPoolHandle)
{
    vk::DescriptorPool descriptorPool = CastObjectToVkObject<vk::DescriptorPool>(_descriptorPoolHandle);
    Backend::DestroyDescriptorPool(m_VulkanContext.device, descriptorPool);
}

void Vulkan::VulkanApp::AllocDescriptorSet(PC_CORE::DescriptorSet* descriptorSets, uint32_t _descriptorSetCount,
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

    VK_CALL(m_VulkanContext.device.allocateDescriptorSets(&descriptorSetAllocateInfo, CastObjectToVkObject<vk::DescriptorSet
            *>(descriptorSets)));
}

void Vulkan::VulkanApp::UpdateDescriptorSet(uint32_t _descriptorWriteCount,
                                            PC_CORE::DescriptorWriteSet* _descriptorWrite)
{
    std::vector<vk::WriteDescriptorSet> vkWriteDescriptorSet(_descriptorWriteCount);
    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
    std::vector<vk::DescriptorImageInfo> descriptorImageInfos;
    std::vector<vk::BufferView> bufferViewTexelBuffer;


    for (uint32_t i = 0; i < _descriptorWriteCount; i++)
    {
        const vk::WriteDescriptorSet vkDescriptorWrite = Backend::RhiToVulkanWriteDescriptorSet(
            _descriptorWrite[i], &descriptorBufferInfos, &descriptorImageInfos, &bufferViewTexelBuffer);

        vkWriteDescriptorSet[i] = vkDescriptorWrite;
    }
    
    
    m_VulkanContext.device.updateDescriptorSets(static_cast<uint32_t>(vkWriteDescriptorSet.size()), vkWriteDescriptorSet.data(), 0, nullptr);
}

void Vulkan::VulkanApp::BindDescriptorSet(PC_CORE::CommandBufferHandle _commandBuffer,
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

PC_CORE::ImageHandle Vulkan::VulkanApp::CreateImage(uint32_t _width, uint32_t _height, uint32_t mipLevels,
                                                    PC_CORE::ImageType _imageType, PC_CORE::RHIFormat _format,
                                                    PC_CORE::ImageTiling _tiling, PC_CORE::RHIImageUsage _usage)
{
    const vk::ImageType imageType = RHIImageToVkImageType(_imageType);
    const vk::Format imageFormat = RHIFormatToVkFormat(_format);
    const vk::ImageTiling tiling = RHiImageToVkImageTiling(_tiling);
    const vk::ImageUsageFlags usageFlags = static_cast<vk::ImageUsageFlags>(_usage);

    vk::Image image = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;

    Vulkan::Backend::CreateImage(&m_VulkanContext, _width, _height, mipLevels, imageType, imageFormat, tiling, usageFlags,
                                 VMA_MEMORY_USAGE_AUTO,
                                 &image, &allocation);

    m_VulkanContext.m_ImagesAllocationMap.insert({VulkanObjectWrapper<vk::Image>(image), allocation});

    return *reinterpret_cast<PC_CORE::ImageHandle*>(&image);
}

void Vulkan::VulkanApp::GenerateMimpMap(PC_CORE::ImageHandle _image, PC_CORE::RHIFormat _imageFormat, int32_t _texWidth,
    int32_t _texHeight, uint32_t _mipLevels)
{
    vk::Image image = CastObjectToVkObject<vk::Image>(_image);
    const vk::Format format = RHIFormatToVkFormat(_imageFormat);
    
    Backend::GenerateMipmaps(image, format, _texWidth, _texHeight, _mipLevels,
        m_VulkanContext.physicalDevice, m_VulkanContext.device, m_VulkanContext.resourceCommandPool, m_VulkanContext.resourceFence, m_VulkanContext.vkQueues.graphicQueue);
}

void Vulkan::VulkanApp::DestroyImage(PC_CORE::ImageHandle _imageHandle)
{
    vk::Image image = CastObjectToVkObject<vk::Image>(_imageHandle);
    VulkanObjectWrapper obj =  VulkanObjectWrapper<vk::Image>(image);
    if (!m_VulkanContext.m_ImagesAllocationMap.contains(obj))
    {
        return;
    }

    auto& ref = m_VulkanContext.m_ImagesAllocationMap.at(obj);
    vmaDestroyImage(m_VulkanContext.allocator,image, ref);
    m_VulkanContext.m_ImagesAllocationMap.erase(obj);
}

void Vulkan::VulkanApp::DestroyImageView(PC_CORE::ImageViewHandle _imageViewHandle)
{
    vk::ImageView imageView = CastObjectToVkObject<vk::ImageView>(_imageViewHandle);

    m_VulkanContext.device.destroyImageView(imageView);
}

PC_CORE::ImageViewHandle Vulkan::VulkanApp::CreateImageView(const PC_CORE::ImageViewCreateInfo& _imageViewCreateInfo)
{
    vk::ImageViewCreateInfo createInfo = {};

    createInfo.sType = vk::StructureType::eImageViewCreateInfo,
        createInfo.pNext = nullptr,
        createInfo.flags = RHIToVulkanImageViewCreateFlags(_imageViewCreateInfo.flags),
        createInfo.image = CastObjectToVkObject<vk::Image>(_imageViewCreateInfo.image),
        createInfo.viewType = RHIImageViewTypeToVulkan(_imageViewCreateInfo.viewType),
        createInfo.format = RHIFormatToVkFormat(_imageViewCreateInfo.format),
        createInfo.components = {
            RHIToVulkanComponentSwizzle(_imageViewCreateInfo.components.r),
            RHIToVulkanComponentSwizzle(_imageViewCreateInfo.components.g),
            RHIToVulkanComponentSwizzle(_imageViewCreateInfo.components.g)
        };

    createInfo.subresourceRange.aspectMask = RhiToVKImageAspectFlagBits(
        _imageViewCreateInfo.subresourceRange.aspectMask);
    createInfo.subresourceRange.baseMipLevel = _imageViewCreateInfo.subresourceRange.baseMipLevel;
    createInfo.subresourceRange.levelCount = _imageViewCreateInfo.subresourceRange.levelCount;
    createInfo.subresourceRange.baseArrayLayer = _imageViewCreateInfo.subresourceRange.baseArrayLayer;
    createInfo.subresourceRange.layerCount = _imageViewCreateInfo.subresourceRange.layerCount;


    PC_CORE::ImageViewHandle imageView = VK_NULL_HANDLE;
    VK_CALL(
        m_VulkanContext.device.createImageView(&createInfo, nullptr, CastObjectToVkObject<vk::ImageView*>(&imageView)));

    return imageView;
}

PC_CORE::SamplerHandle Vulkan::VulkanApp::CreateSampler(const PC_CORE::SamplerCreateInfo& _samplerCreateInfo)
{
    vk::SamplerCreateInfo createInfo = {};
    createInfo.sType = vk::StructureType::eSamplerCreateInfo;
    createInfo.pNext = nullptr;
    createInfo.flags = RHIToVulkanSamplerCreateInfoFlags(_samplerCreateInfo.flags);
    createInfo.magFilter = RHIToVulkanFilter(_samplerCreateInfo.magFilter);
    createInfo.minFilter = RHIToVulkanFilter(_samplerCreateInfo.minFilter);
    createInfo.mipmapMode = RHIToSamplerMipmapMode(_samplerCreateInfo.mipmapMode);
    createInfo.addressModeU = RHIToVulkanSamplerAddressMode(_samplerCreateInfo.addressModeU);
    createInfo.addressModeV = RHIToVulkanSamplerAddressMode(_samplerCreateInfo.addressModeV);
    createInfo.addressModeW = RHIToVulkanSamplerAddressMode(_samplerCreateInfo.addressModeW);
    createInfo.mipLodBias = _samplerCreateInfo.mipLodBias;
    createInfo.anisotropyEnable = _samplerCreateInfo.anisotropyEnable ? vk::True : vk::False;
    createInfo.maxAnisotropy = m_VulkanContext.physicalDeviceProperties.limits.maxSamplerAnisotropy;
    createInfo.compareEnable = _samplerCreateInfo.compareEnable ? vk::True : vk::False;
    createInfo.compareOp = RHIToVulkanCompareOp(_samplerCreateInfo.compareOp);
    createInfo.minLod = _samplerCreateInfo.minLod;
    createInfo.maxLod = _samplerCreateInfo.maxLod;
    createInfo.borderColor = RHIToBorderColor(_samplerCreateInfo.borderColor);
    createInfo.unnormalizedCoordinates = _samplerCreateInfo.unnormalizedCoordinates ? vk::True : vk::False;
    
    
    PC_CORE::SamplerHandle samplerHandle;
    VK_CALL(m_VulkanContext.device.createSampler(&createInfo, nullptr,CastObjectToVkObject<vk::Sampler*>(&samplerHandle)));

    return samplerHandle;
}

void Vulkan::VulkanApp::DestroySampler(PC_CORE::SamplerHandle _samplerHandle)
{
    m_VulkanContext.device.destroySampler(CastObjectToVkObject<vk::Sampler>(_samplerHandle));
}

void Vulkan::VulkanApp::CopyBufferToImage(PC_CORE::GPUBufferHandle _buffer, PC_CORE::ImageHandle _image,
                                          const PC_CORE::CopyBufferImageInfo&
                                          _copyBufferImageInfo)
{
    vk::Buffer buffer = CastObjectToVkObject<vk::Buffer>(_buffer);
    vk::Image image = CastObjectToVkObject<vk::Image>(_image);
    vk::BufferImageCopy copyRegion = {};
    copyRegion.bufferOffset = _copyBufferImageInfo.bufferOffset;
    copyRegion.bufferRowLength = _copyBufferImageInfo.bufferRowLength;
    copyRegion.bufferImageHeight = _copyBufferImageInfo.bufferImageHeight;

    copyRegion.imageSubresource.aspectMask = RhiToVKImageAspectFlagBits(
            _copyBufferImageInfo.imageSubresource.aspectMask),
        copyRegion.imageSubresource.mipLevel = _copyBufferImageInfo.imageSubresource.mipLevel,
        copyRegion.imageSubresource.baseArrayLayer = _copyBufferImageInfo.imageSubresource.baseArrayLayer,
        copyRegion.imageSubresource.layerCount = _copyBufferImageInfo.imageSubresource.layerCount,

        copyRegion.imageOffset = vk::Offset3D(_copyBufferImageInfo.imageOffset3D[0],
                                              _copyBufferImageInfo.imageOffset3D[1],
                                              _copyBufferImageInfo.imageOffset3D[2]);
    copyRegion.imageExtent = vk::Extent3D(_copyBufferImageInfo.imageExtent3D[0], _copyBufferImageInfo.imageExtent3D[1],
                                          _copyBufferImageInfo.imageExtent3D[2]);

    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(m_VulkanContext.device,
                                                              m_VulkanContext.resourceCommandPool);

    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &copyRegion);

    EndSingleTimeCommands(commandBuffer, m_VulkanContext.device, m_VulkanContext.resourceFence,
                          m_VulkanContext.vkQueues.graphicQueue);
}

void Vulkan::VulkanApp::CopyBuffer(PC_CORE::GPUBufferHandle _bufferSrc, PC_CORE::GPUBufferHandle _bufferDst,
                                   size_t _srcOffset, size_t _dstOffset, size_t _size)
{
    vk::Buffer bufferSrc = CastObjectToVkObject<vk::Buffer>(_bufferSrc);
    vk::Buffer bufferDST = CastObjectToVkObject<vk::Buffer>(_bufferSrc);

    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(m_VulkanContext.device,
                                                              m_VulkanContext.resourceCommandPool);

    vk::BufferCopy copyRegion{};
    copyRegion.srcOffset = _srcOffset;
    copyRegion.dstOffset = _dstOffset;
    copyRegion.size = _size;

    commandBuffer.copyBuffer(bufferSrc, bufferDST, copyRegion);

    EndSingleTimeCommands(commandBuffer, m_VulkanContext.device, m_VulkanContext.resourceFence,
                          m_VulkanContext.vkQueues.graphicQueue);
}

void Vulkan::VulkanApp::TransitionImageLayout(PC_CORE::ImageHandle _imageHandle,
                                              PC_CORE::ImageAspectFlagBits _imageAspectFlagBits,
                                              PC_CORE::RHIFormat _format, uint32_t _mipLevel, PC_CORE::ImageLayout _initialLayout,
                                              PC_CORE::ImageLayout _finalLayout)
{
    const vk::ImageLayout initialLayout = RHIToVKImageLayout(_initialLayout);
    const vk::ImageLayout finalLayout = RHIToVKImageLayout(_finalLayout);

    vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(m_VulkanContext.device,
                                                              m_VulkanContext.resourceCommandPool);

    vk::ImageMemoryBarrier barrier{};
    barrier.sType = vk::StructureType::eImageMemoryBarrier;
    barrier.oldLayout = initialLayout;
    barrier.newLayout = finalLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = CastObjectToVkObject<vk::Image>(_imageHandle);
    barrier.subresourceRange.aspectMask = RhiToVKImageAspectFlagBits(_imageAspectFlagBits);
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = _mipLevel;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    // TO DO MAKE IT CLEANER
    if (initialLayout == vk::ImageLayout::eUndefined && finalLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eNone;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (initialLayout == vk::ImageLayout::eTransferDstOptimal && finalLayout ==
        vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    // vk::DependencyFlagBits() ???
    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr, 0, nullptr, 1,
                                  &barrier);

    EndSingleTimeCommands(commandBuffer, m_VulkanContext.device, m_VulkanContext.resourceFence,
                          m_VulkanContext.vkQueues.graphicQueue);
}

PC_CORE::RenderPassHandle Vulkan::VulkanApp::CreateRenderPass(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo)
{
    std::vector<vk::AttachmentDescription> colorAttachmentsDescriptions{};
    colorAttachmentsDescriptions.resize(_renderPassCreateInfo.attachmentDescriptions.size());
    
    for (size_t i = 0; i < _renderPassCreateInfo.attachmentDescriptions.size(); i++)
    {
        colorAttachmentsDescriptions[i].format = RHIFormatToVkFormat(_renderPassCreateInfo.attachmentDescriptions[i].format);  
        colorAttachmentsDescriptions[i].samples = vk::SampleCountFlagBits::e1;
            
        switch (_renderPassCreateInfo.attachmentDescriptions[i].renderPassTargetType)
        {
        case PC_CORE::AttachementUsage::NONE:
            break;

        case  PC_CORE::AttachementUsage::DEPTH:
        case PC_CORE::AttachementUsage::COLOR:
            colorAttachmentsDescriptions[i].loadOp = vk::AttachmentLoadOp::eClear;
            colorAttachmentsDescriptions[i].storeOp = vk::AttachmentStoreOp::eStore;
                
            colorAttachmentsDescriptions[i].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
            colorAttachmentsDescriptions[i].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            break;
        case PC_CORE::AttachementUsage::STENCIL:
            colorAttachmentsDescriptions[i].loadOp = vk::AttachmentLoadOp::eDontCare;
            colorAttachmentsDescriptions[i].storeOp = vk::AttachmentStoreOp::eDontCare;
                
            colorAttachmentsDescriptions[i].stencilLoadOp = vk::AttachmentLoadOp::eClear;
            colorAttachmentsDescriptions[i].stencilStoreOp = vk::AttachmentStoreOp::eStore;
            break;
        case PC_CORE::AttachementUsage::COUNT:
            break;
        default: ;
        }
  
        colorAttachmentsDescriptions[i].initialLayout = vk::ImageLayout::eUndefined;
        colorAttachmentsDescriptions[i].finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
    }

    const uint32_t subPassesSize = static_cast<uint32_t>(_renderPassCreateInfo.subPasses.size());
    
    std::vector<vk::AttachmentReference> attachmentReferences{};
    attachmentReferences.resize(subPassesSize);

    std::vector<vk::SubpassDescription> subpasses{};
    subpasses.resize(subPassesSize);

    std::vector<vk::SubpassDependency> subpassDependencies{};
    subpasses.resize(subPassesSize);

    for (size_t i = 0; i < subPassesSize; i++)
    {
        attachmentReferences[i].attachment = static_cast<uint32_t>(i);
        switch (_renderPassCreateInfo.subPasses[i].renderPassTargetType)
        {
        case PC_CORE::AttachementUsage::COLOR:
            attachmentReferences[i].layout = vk::ImageLayout::eColorAttachmentOptimal;
            break;
        case PC_CORE::AttachementUsage::DEPTH:
            attachmentReferences[i].layout = vk::ImageLayout::eDepthAttachmentOptimal;
            break;
        case PC_CORE::AttachementUsage::STENCIL:
            attachmentReferences[i].layout = vk::ImageLayout::eStencilAttachmentOptimal;
            break;
        }

        
    }

    /*
    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;  
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = {};
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = colorAttachmentsDescriptions.data();
    renderPassInfo.subpassCount = static_cast<uint32_t>(colorAttachmentsDescriptions.size());
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;  

   return m_VulkanContext.device.createRenderPass(renderPassInfo);*/
    return nullptr;
}


void Vulkan::VulkanApp::DestroyRenderPass(PC_CORE::RenderPassHandle _renderPassHandle)
{
    
}

#pragma endregion DescriptorSetLayout


#pragma endregion CommandBuffer Functions

#pragma region CommandPool Functions


#pragma endregion CommandPool Functions

#pragma region InitBaseObject

void Vulkan::VulkanApp::InitBaseObject()
{
    // resource Command Pool
    vk::CommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    commandPoolCreateInfo.queueFamilyIndex = m_VulkanContext.queuFamiliesIndicies.graphicsFamily;
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
    m_VulkanContext.resourceCommandPool = m_VulkanContext.device.createCommandPool(commandPoolCreateInfo, nullptr);

    // resource resource fence
    vk::FenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
    m_VulkanContext.resourceFence = m_VulkanContext.device.createFence(fenceCreateInfo);
}

void Vulkan::VulkanApp::DestroyBaseObject()
{
    m_VulkanContext.device.destroyCommandPool(m_VulkanContext.resourceCommandPool);
    m_VulkanContext.device.destroyFence(m_VulkanContext.resourceFence);
}

void Vulkan::VulkanApp::DestroyBuffersAllocations(VulkanContext* _vulkanContext)
{
    for (auto&& buffer : m_VulkanContext.m_BuffersAllocationMap)
    {
        vmaDestroyBuffer(m_VulkanContext.allocator, buffer.first.object, buffer.second);
    }
}

void Vulkan::VulkanApp::DestroyImagesAllocations(VulkanContext* _vulkanContext)
{
    for (auto&& buffer : m_VulkanContext.m_ImagesAllocationMap)
    {
        vmaDestroyImage(m_VulkanContext.allocator, buffer.first.object, buffer.second);
    }
}
#pragma endregion InitBaseObject
