#include "VulkanCommandList.hpp"

#include "PerfRegion.hpp"

#include "LowRenderer/Rhi.hpp"
#include "VulkanContext.hpp"
#include "VulkanFrameBuffer.hpp"
#include "VulkanRenderPass.hpp"
#include "Resources/VulkanShaderProgram.hpp"
#include "Resources/VulkanDescriptorSets.hpp"
#include "VulkanFence.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"

#include "Utils/RhiToVulkan.hpp"

Vulkan::VulkanCommandList::VulkanCommandList(PC_CORE::Rhi& _Rhi)
    : CommandList(_Rhi)
{

}

Vulkan::VulkanCommandList::~VulkanCommandList()
{
    vk::Device device = GET_VK_DEVICE;

    for (auto& s : m_Semaphore)
    {
        if (s != VK_NULL_HANDLE)
            device.destroySemaphore(s);
    }

#ifdef PROFILING
    tracy::DestroyVkContext(tracyContext);
    tracyContext = nullptr;
#endif
}

const void* Vulkan::VulkanCommandList::GetFrameNativeHandle(size_t _frameIndex) const
{
    const uint32_t frameIndex = m_Rhi.GetFrameIndex();
    return &m_CommandBuffer[frameIndex];
}

void* Vulkan::VulkanCommandList::GetFrameNativeHandle(size_t _frameIndex)
{
    const uint32_t frameIndex = m_Rhi.GetFrameIndex();
    return &m_CommandBuffer[frameIndex];
}

bool Vulkan::VulkanCommandList::Build() 
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    VulkanContext& vulkanContext = GET_VK_CONTEXT;
    vk::Device device = GET_VK_DEVICE;

    vk::CommandPool commandPool = VK_NULL_HANDLE;

    switch (m_PoolFamily)
    {
    case PC_CORE::CommandList::PoolFamily::Graphics:
        commandPool = vulkanContext.commandPool;
        break;
    case PC_CORE::CommandList::PoolFamily::Compute:
        break;
    case PC_CORE::CommandList::PoolFamily::Count:
        break;
    default:;
    }

    if (commandPool == VK_NULL_HANDLE)
    {
        PC_LOGERROR("Invalid PoolFamily");
        return false;
    }

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = m_BufferType == PC_CORE::CommandList::BufferType::Primary
        ? vk::CommandBufferLevel::ePrimary
        : vk::CommandBufferLevel::eSecondary;

    commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffer.size());

    std::vector<vk::CommandBuffer> vec = device.allocateCommandBuffers(commandBufferAllocateInfo);

    for (uint32_t i = 0; i < m_CommandBuffer.size(); i++)
    {
        m_CommandBuffer[i] = vec[i];

        vk::DebugUtilsObjectNameInfoEXT nameInfoImageView;
        nameInfoImageView.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
        nameInfoImageView.pNext = nullptr;
        nameInfoImageView.objectType = vk::ObjectType::eCommandBuffer;
        nameInfoImageView.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkCommandBuffer>(m_CommandBuffer[i]));
        nameInfoImageView.pObjectName = GetName().data();
        SET_VK_DEBUG_NAME(nameInfoImageView);
    }

    vk::SemaphoreCreateInfo sCreateInfo;
    sCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

#if DEBUG_GPU_ON

    m_SemaphoreDebugName = std::move(std::string(GetName()) + " Semaphore");
    for (auto& s : m_Semaphore)
    {
        s = device.createSemaphore(sCreateInfo);
        vk::DebugUtilsObjectNameInfoEXT nameInfoImageView;
        nameInfoImageView.sType = vk::StructureType::eDebugUtilsObjectNameInfoEXT;
        nameInfoImageView.pNext = nullptr;
        nameInfoImageView.objectType = vk::ObjectType::eSemaphore;
        nameInfoImageView.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkSemaphore>(s));
        nameInfoImageView.pObjectName = m_SemaphoreDebugName.c_str();
        SET_VK_DEBUG_NAME(nameInfoImageView);
    }
#endif

#ifdef PROFILING
    vk::PhysicalDevice physDv = vulkanContext.GetPhysicalDevices()->GetVulkanDevice();
    VulkanInstance& instance = *std::reinterpret_pointer_cast<VulkanInstance>(vulkanContext.renderInstance).get();
    VulkanDevice& vulkanDevice = *GET_VK_DEVICE.get();

    tracyContext = tracy::CreateVkContext(physDv, device,
        vulkanDevice.GetPFN_vkResetQueryPoolEXT(),
        instance.GetPFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(),
        vulkanDevice.GetPFN_vkGetCalibratedTimestampsEXT());

#endif

    return true;
}

void Vulkan::VulkanCommandList::Reset()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    m_CommandBuffer[m_Rhi.GetFrameIndex()].reset();
}

void Vulkan::VulkanCommandList::MergeCommands(CommandList* _other, size_t _count)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    assert(_count != 0);
    assert(this != _other);
    assert(m_BufferType == PC_CORE::CommandList::BufferType::Primary);

    vk::CommandBuffer* commandBuffers = reinterpret_cast<vk::CommandBuffer*>(_malloca(sizeof(vk::CommandBuffer) * _count));

    for (size_t i = 0; i < _count; i++)
    {
        auto otherVk = reinterpret_cast<VulkanCommandList*>(_other);
        commandBuffers[i] = otherVk->m_CommandBuffer[m_Rhi.GetFrameIndex()];
    }

    m_CommandBuffer[m_Rhi.GetFrameIndex()].executeCommands(static_cast<uint32_t>(_count), commandBuffers);
    _freea(commandBuffers);
}

void Vulkan::VulkanCommandList::BeginRecordCommands()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const uint32_t frameIndex = m_Rhi.GetFrameIndex();

    vk::CommandBufferInheritanceInfo inheritanceInfo;
    if (m_BufferType == PC_CORE::CommandList::BufferType::Secondary)
    {
        inheritanceInfo.sType = vk::StructureType::eCommandBufferInheritanceInfo;
        inheritanceInfo.pNext = nullptr;
        inheritanceInfo.renderPass = VK_NULL_HANDLE;
        inheritanceInfo.subpass = {};
        inheritanceInfo.framebuffer = VK_NULL_HANDLE;
        inheritanceInfo.occlusionQueryEnable = {};
        inheritanceInfo.queryFlags = {};
        inheritanceInfo.pipelineStatistics = {};
    }


    vk::CommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits(); // Optional
    commandBufferBeginInfo.pInheritanceInfo = m_BufferType == PC_CORE::CommandList::BufferType::Secondary
                                                  ? &inheritanceInfo
                                                  : nullptr; // Optional

    m_CommandBuffer[frameIndex].begin(commandBufferBeginInfo);
}

void Vulkan::VulkanCommandList::EndRecordCommands()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    m_CommandBuffer[m_Rhi.GetFrameIndex()].end();
}

void Vulkan::VulkanCommandList::BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::shared_ptr<const VulkanFrameBuffer> frameBuffer = std::reinterpret_pointer_cast<const VulkanFrameBuffer>(
        _BeginRenderPassInfo.FrameBuffer);
    std::shared_ptr<const VulkanRenderPass> renderPass = std::reinterpret_pointer_cast<const VulkanRenderPass>(
        _BeginRenderPassInfo.RenderPass);


    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassInfo.renderPass = renderPass->GetVulkanRenderPass();
    renderPassInfo.framebuffer = frameBuffer->GetFramebuffer();
    renderPassInfo.renderArea.offset = vk::Offset2D{
        _BeginRenderPassInfo.RenderOffSet.x, _BeginRenderPassInfo.RenderOffSet.y
    };
    renderPassInfo.renderArea.extent = vk::Extent2D{_BeginRenderPassInfo.Extent.x, _BeginRenderPassInfo.Extent.y};

    assert((renderPassInfo.renderArea.offset.x + renderPassInfo.renderArea.extent.width) <= frameBuffer->GetWidth());
    assert((renderPassInfo.renderArea.offset.y + renderPassInfo.renderArea.extent.height) <= frameBuffer->GetHeight());


    constexpr size_t MaxClearValues = 10;
    size_t clearCount = 0;
    std::array<vk::ClearValue, MaxClearValues> clearValues;
    assert(
        _BeginRenderPassInfo.ClearValueCount < clearValues.size() &&
        "clearValues.size() should be graeter than _BeginRenderPassInfo.clearValueCount");

    if ((_BeginRenderPassInfo.ClearValueFlag & PC_CORE::ClearValueFlagBits::ClearValueColor) &&
        (_BeginRenderPassInfo.ClearValueFlag & PC_CORE::ClearValueFlagBits::ClearValueDepth))
    {
        // Clear + Depht
        clearCount = _BeginRenderPassInfo.ClearValueCount + 1;
        for (size_t i = 0; i < _BeginRenderPassInfo.ClearValueCount; i++)
        {
            clearValues[0].color.setFloat32({
                _BeginRenderPassInfo.ClearColor[i].x,
                _BeginRenderPassInfo.ClearColor[i].y,
                _BeginRenderPassInfo.ClearColor[i].z,
                _BeginRenderPassInfo.ClearColor[i].w
            });
        }

        clearValues[clearCount - 1].depthStencil.setDepth(_BeginRenderPassInfo.ClearDepth);
    }
    else if (_BeginRenderPassInfo.ClearValueFlag & PC_CORE::ClearValueFlagBits::ClearValueDepth)
    {
        clearValues[0].depthStencil.setDepth(_BeginRenderPassInfo.ClearDepth);
        clearCount = 1;
    }
    else if (_BeginRenderPassInfo.ClearValueFlag & PC_CORE::ClearValueFlagBits::ClearValueColor)
    {
        clearCount = _BeginRenderPassInfo.ClearValueCount;
        for (size_t i = 0; i < _BeginRenderPassInfo.ClearValueCount; i++)
        {
            clearValues[0].color.setFloat32({
                _BeginRenderPassInfo.ClearColor[i].x,
                _BeginRenderPassInfo.ClearColor[i].y,
                _BeginRenderPassInfo.ClearColor[i].z,
                _BeginRenderPassInfo.ClearColor[i].w
            });
        }
    }
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearCount);
    renderPassInfo.pClearValues = clearValues.data();

    m_CommandBuffer[m_Rhi.GetFrameIndex()].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void Vulkan::VulkanCommandList::NextSubPass()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    m_CommandBuffer[m_Rhi.GetFrameIndex()].nextSubpass(vk::SubpassContents::eInline);
}

void Vulkan::VulkanCommandList::EndRenderPass()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    m_CommandBuffer[m_Rhi.GetFrameIndex()].endRenderPass();
}

void Vulkan::VulkanCommandList::BindDescriptorSet(const PC_CORE::RhiShaderProgram& _RhiShaderProgram,
                                                  const PC_CORE::ShaderProgramDescriptorSets*
                                                  _shaderProgramDescriptorSets, size_t _firstSet,
                                                  size_t _descriptorSetCount)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const size_t currentFrame = m_Rhi.GetFrameIndex();

    const VulkanShaderProgram& shaderProgram = reinterpret_cast<const VulkanShaderProgram&>(_RhiShaderProgram);
    const VulkanDescriptorSets* vulkanDescriptorSets = reinterpret_cast<const VulkanDescriptorSets*>(_shaderProgramDescriptorSets);
    const vk::DescriptorSet& descriptorHandles = *static_cast<const vk::DescriptorSet*>(vulkanDescriptorSets->GetFrameNativeHandle(currentFrame));
    
    m_CommandBuffer[currentFrame].bindDescriptorSets(shaderProgram.GetPipelineBindPoint(),
                                                     shaderProgram.GetPipelineLayout(),
                                                     static_cast<uint32_t>(_firstSet), 
                                                     static_cast<uint32_t>(_descriptorSetCount),
                                                     &descriptorHandles,
                                                     0, nullptr);
}

void Vulkan::VulkanCommandList::BindProgram(const PC_CORE::RhiShaderProgram& _RhiShaderProgram)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const VulkanShaderProgram& vshadeProgram = reinterpret_cast<const VulkanShaderProgram&>(_RhiShaderProgram);
    
    m_CommandBuffer[m_Rhi.GetFrameIndex()].bindPipeline(vshadeProgram.GetPipelineBindPoint(),
                                                                vshadeProgram.GetPipeline());
}

void Vulkan::VulkanCommandList::PushConstant(const PC_CORE::RhiShaderProgram& _RhiShaderProgram,
                                             const std::string& _pushConstantKey, const void* _data, const size_t _size)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    const VulkanShaderProgram& vshadeProgram = reinterpret_cast<const VulkanShaderProgram&>(_RhiShaderProgram);

    vshadeProgram.PushConstant(GetVkHandle(), _pushConstantKey, _data, _size);
}

void Vulkan::VulkanCommandList::SetViewPort(const PC_CORE::ViewportInfo& _viewPort)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    vk::Viewport viewport{};
    viewport.x = _viewPort.Transform.x;
    viewport.y = _viewPort.Transform.y;
    viewport.width = static_cast<float>(_viewPort.Size.x);
    viewport.height = static_cast<float>(_viewPort.Size.y);
    viewport.minDepth = _viewPort.MinDepth;
    viewport.maxDepth = _viewPort.MaxDepth;
    m_CommandBuffer[m_Rhi.GetFrameIndex()].setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{_viewPort.ScissorsOff.x, _viewPort.ScissorsOff.y};
    scissor.extent = vk::Extent2D{_viewPort.ScissorsExtent.x, _viewPort.ScissorsExtent.y};

    m_CommandBuffer[m_Rhi.GetFrameIndex()].setScissor(0, 1, &scissor);
}

void Vulkan::VulkanCommandList::SetPrimitiveTopology(PC_CORE::RhiShaderProgram::PrimitiveTopology _primitiveTopology)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    vk::PrimitiveTopology topology = Utils::RhiPrimitiveTopology(_primitiveTopology);
    m_CommandBuffer[m_Rhi.GetFrameIndex()].setPrimitiveTopology(topology);
}

void Vulkan::VulkanCommandList::SetBlendEquation(uint32_t _firstAttachement, uint32_t _attachementCount)
{
    // TO DO
    /*
    const vk::ColorBlendEquationEXT blendEq =
        {
        .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
        .dstColorBlendFactor = vk::BlendFactor::eOneMinusConstantAlpha,
        .colorBlendOp = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp = vk::BlendOp::eAdd,
        };

    m_CommandBuffer[m_Rhi.GetFrameIndex()].setColorBlendEquationEXT(_firstAttachement, _attachementCount, &blendEq);*/
}

void Vulkan::VulkanCommandList::SetLineWidth(float _widht)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    m_CommandBuffer[m_Rhi.GetFrameIndex()].setLineWidth(_widht);
}

void Vulkan::VulkanCommandList::Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex,
                                     uint32_t _firstInstance)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    m_CommandBuffer[m_Rhi.GetFrameIndex()].draw(_vertexCount, _instanceCount, _firstVertex, _firstInstance);
}

void Vulkan::VulkanCommandList::DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex,
                                            int32_t _vertexOffset, size_t _firstInstance)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    m_CommandBuffer[m_Rhi.GetFrameIndex()].drawIndexed(static_cast<uint32_t>(_indexCount),
                                                               static_cast<uint32_t>(_instanceCount),
                                                               static_cast<uint32_t>(_firstIndex), _vertexOffset,
                                                               static_cast<uint32_t>(_firstInstance));
}

void Vulkan::VulkanCommandList::Dispatch(uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    m_CommandBuffer[m_Rhi.GetFrameIndex()].dispatch(_groupCountX, _groupCountY, _groupCountZ);
}

void Vulkan::VulkanCommandList::BindVertexBuffer(const PC_CORE::RhiBuffer& _vertexBuffer, uint32_t _firstBinding,
                                                 uint32_t _bindingCount)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    const size_t frameIndex = m_Rhi.GetFrameIndex();

    const VulkanBuffer* vulkanBuffer = reinterpret_cast<const VulkanBuffer*>(&_vertexBuffer);
    const BufferAndAlloc* bufferAndAllocs = static_cast<const BufferAndAlloc*>(vulkanBuffer->GetFrameNativeHandle(frameIndex));

    vk::DeviceSize offsets[] = {0};
    m_CommandBuffer[frameIndex].bindVertexBuffers(_firstBinding, _bindingCount, &bufferAndAllocs->buffer,
                                                  offsets);
}

void Vulkan::VulkanCommandList::BindIndexBuffer(const PC_CORE::RhiBuffer& _indexBuffer, PC_CORE::RhiBuffer::IndexFormat _format, size_t _offset)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const size_t frameIndex = m_Rhi.GetFrameIndex();
    const VulkanBuffer* vulkanBuffer = reinterpret_cast<const VulkanBuffer*>(&_indexBuffer);
    const BufferAndAlloc* bufferAndAllocs = static_cast<const BufferAndAlloc*>(vulkanBuffer->GetFrameNativeHandle(frameIndex));

    const vk::IndexType indexType = Utils::RhiToIndexType(_format);

    m_CommandBuffer[frameIndex].bindIndexBuffer(bufferAndAllocs->buffer, static_cast<uint32_t>(_offset),
                                                indexType);
}

void Vulkan::VulkanCommandList::CopyBuffer(const PC_CORE::RhiBuffer& _src, const PC_CORE::RhiBuffer& _dst,
                                           size_t _srcOffSet, size_t _dstoffset, size_t _sizeInBytes)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    
    assert(_src.GetMemoryUsage() == PC_CORE::RhiResource::MemoryUsage::Dynamic && _src.GetMemoryUsage() == _dst.GetMemoryUsage()
        && "This function only work for dynamic for now");
    
    const size_t frameIndex = m_Rhi.GetFrameIndex();

    const BufferAndAlloc* bufferAndAllocSrc = static_cast<const BufferAndAlloc*>(_src.GetFrameNativeHandle(frameIndex));
    const BufferAndAlloc* bufferAndAllocDst = static_cast<const BufferAndAlloc*>(_dst.GetFrameNativeHandle(frameIndex));

    vk::Buffer bufferSrc = bufferAndAllocSrc->buffer;
    vk::Buffer bufferDst = bufferAndAllocDst->buffer;

    vk::BufferCopy bufferCopy = {};
    bufferCopy.dstOffset = static_cast<uint32_t>(_dstoffset);
    bufferCopy.srcOffset = static_cast<uint32_t>(_srcOffSet);
    bufferCopy.size = static_cast<uint32_t>(_sizeInBytes);


    m_CommandBuffer[frameIndex].copyBuffer(bufferSrc, bufferDst, 1, &bufferCopy);
}

VULKAN_API void Vulkan::VulkanCommandList::Barrier(PC_CORE::GpuPipelineStage _srcStageMask, PC_CORE::GpuPipelineStage _DstStageMask,
    const std::span<PC_CORE::ImageStateTransition>& _ImageStateTransition,
    const std::span<PC_CORE::BufferStateTransition>& _BufferStateTransition)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    const vk::PipelineStageFlags srcStageFlag = Utils::RhiPipelineStageToVulkan(_srcStageMask);
    const vk::PipelineStageFlags dstStageFlag = Utils::RhiPipelineStageToVulkan(_DstStageMask);

    /*// Memory Barrier
    std::vector<vk::MemoryBarrier> vkMemoryBarriers;
    vkMemoryBarriers.resize(_memoryBarrierCount);
    for (size_t i = 0; i < _memoryBarrierCount; i++)
    {
        vkMemoryBarriers[i].sType = vk::StructureType::eMemoryBarrier;
        vkMemoryBarriers[i].pNext = nullptr;
        vkMemoryBarriers[i].srcAccessMask = Utils::RhiAccessFlagToVulkan(_memoryBarrier[i].SrcAccessMask);
        vkMemoryBarriers[i].dstAccessMask = Utils::RhiAccessFlagToVulkan(_memoryBarrier[i].DstAccessMask);
    }*/
    // BufferMemoryBarrier


    const size_t frameIndex = m_Rhi.GetFrameIndex();

    m_VkImageBarrier.clear();
    m_VkBufferBarrier.clear();

    m_VkImageBarrier.resize(_ImageStateTransition.size());
    for (size_t i = 0; i < _ImageStateTransition.size(); i++)
    {
        PC_CORE::RhiTexture* texture = _ImageStateTransition[i].Texture;

        const VulkanTexture* vulkanTexture = static_cast<VulkanTexture*>(texture);
        const TextureAndAlloc* textureAndAlloc = static_cast<const TextureAndAlloc*>(vulkanTexture->GetFrameNativeHandle(frameIndex));

        vk::ImageMemoryBarrier& bar = m_VkImageBarrier[i];

        bar.sType = vk::StructureType::eImageMemoryBarrier;
        bar.pNext = nullptr;
        bar.image = textureAndAlloc->Image;
        bar.srcAccessMask = Utils::RhiResourceStateToAccesFlag(_ImageStateTransition[i].OldState);
        bar.dstAccessMask = Utils::RhiResourceStateToAccesFlag(_ImageStateTransition[i].NewState);
        bar.oldLayout = Utils::RhiResourceStateToVulkanImageLayout(_ImageStateTransition[i].OldState);
        bar.newLayout = Utils::RhiResourceStateToVulkanImageLayout(_ImageStateTransition[i].NewState);
        bar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;


        vk::ImageSubresourceRange& ImageSubresourceRange = bar.subresourceRange;
        ImageSubresourceRange.aspectMask = vulkanTexture->VkImageAspectFlags;

        ImageSubresourceRange.baseMipLevel = _ImageStateTransition[i].FirstMipLevel;
        ImageSubresourceRange.levelCount = _ImageStateTransition[i].MipLevelsCount;

        ImageSubresourceRange.baseArrayLayer = _ImageStateTransition[i].FirstArraySlice;
        ImageSubresourceRange.layerCount = _ImageStateTransition[i].ArraySliceCount;
    }

    m_VkBufferBarrier.resize(_BufferStateTransition.size());

    for (size_t i = 0; i < _BufferStateTransition.size(); i++)
    {
        const BufferAndAlloc* bufferAndAlloc = static_cast<const BufferAndAlloc*>(_BufferStateTransition[i].Buffer->GetFrameNativeHandle(frameIndex));

        m_VkBufferBarrier[i].sType = vk::StructureType::eBufferMemoryBarrier;
        m_VkBufferBarrier[i].pNext = nullptr;
        m_VkBufferBarrier[i].srcAccessMask = Utils::RhiResourceStateToAccesFlag(_BufferStateTransition[i].OldState);
        m_VkBufferBarrier[i].dstAccessMask = Utils::RhiResourceStateToAccesFlag(_BufferStateTransition[i].NewState);
        m_VkBufferBarrier[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        m_VkBufferBarrier[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        m_VkBufferBarrier[i].buffer = bufferAndAlloc->buffer;
        m_VkBufferBarrier[i].offset = _BufferStateTransition[i].Offset;
        m_VkBufferBarrier[i].size = _BufferStateTransition[i].Size;
    }
    
    m_CommandBuffer[frameIndex].pipelineBarrier(srcStageFlag, dstStageFlag , {},
        0, nullptr,
        static_cast<uint32_t>(m_VkBufferBarrier.size()), m_VkBufferBarrier.data(),
        static_cast<uint32_t>(m_VkImageBarrier.size()), m_VkImageBarrier.data());
}



/*
void Vulkan::VulkanCommandList::Flush(PC_CORE::RhiFence& _fence)
{
    const size_t frameIndex = m_Rhi.GetFrameIndex();
    vk::CommandBuffer cmb = m_CommandBuffer[frameIndex];

    vk::Fence vkfence = *static_cast<vk::Fence*>(static_cast<VulkanFence&>(_fence).GetFrameNativeHandle(frameIndex));
  
    vk::SubmitInfo submitInfo;
    submitInfo.pCommandBuffers = &cmb;
    submitInfo.commandBufferCount = 1;
    
    VulkanContext& vkContext = GET_VK_CONTEXT;


    VK_CALL(vkContext.mainQueue.submit(1, &submitInfo, vkfence));

    auto device = vkContext.GetDevice()->GetDevice();
    VK_CALL(device.waitForFences(1, &vkfence, vk::True, UINT64_MAX));
    VK_CALL(device.resetFences(1, &vkfence));
}*/

vk::CommandBuffer Vulkan::VulkanCommandList::GetVkHandle() const
{
    return m_CommandBuffer[m_Rhi.GetFrameIndex()];
}

VULKAN_API vk::Semaphore Vulkan::VulkanCommandList::GetVkSemaphore() const
{
    return m_Semaphore[m_Rhi.GetFrameIndex()];
}

void Vulkan::VulkanCommandList::BeginDebugLabel(const char* _debugLabel, const std::array<float, 4>& _color)
{
#ifdef  DEBUG_GPU_ON
    VkDebugUtilsLabelEXT markerInfo = {};
    markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    markerInfo.pLabelName = _debugLabel;

    std::memcpy(&markerInfo.color[0], _color.data(), sizeof(float) * _color.size());
    static auto begindDebugLabelPtrFunc = std::reinterpret_pointer_cast<VulkanInstance>(
        m_Rhi.GetRhiContext().renderInstance)->GetPFN_vkCmdBeginDebugUtilsLabelEXT();
    begindDebugLabelPtrFunc(m_CommandBuffer[m_Rhi.GetFrameIndex()], &markerInfo);
#endif
}

void Vulkan::VulkanCommandList::EndDebugLabel()
{
#ifdef  DEBUG_GPU_ON
    static auto endDebugLabelPtrFunc = std::reinterpret_pointer_cast<VulkanInstance>(
        m_Rhi.GetRhiContext().renderInstance)->GetPFN_vkCmdEndDebugUtilsLabelEXT();
    endDebugLabelPtrFunc(m_CommandBuffer[m_Rhi.GetFrameIndex()]);
#endif
}
