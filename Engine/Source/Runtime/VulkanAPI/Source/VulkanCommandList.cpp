#include <VulkanCommandList.hpp>

#include <PerfRegion.hpp>
#include <LowRenderer/Rhi.hpp>
#include <VulkanBuffer.hpp>
#include <VulkanContext.hpp>
#include <VulkanDescriptorSet.hpp>
#include <VulkanFence.hpp>
#include <VulkanFrameBuffer.hpp>
#include <VulkanRenderPass.hpp>
#include <VulkanGraphicPipeline.hpp>
#include <VulkanComputePipeline.hpp>
#include <VulkanTexture.hpp>
#include <Utils/RhiToVulkan.hpp>

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
    if (tracyContext != nullptr)
    {
        tracy::DestroyVkContext(tracyContext);
        tracyContext = nullptr;
    }
#endif
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

    {
        std::scoped_lock _(vulkanContext.VulkanContextMutex());
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
#else 
        for (auto& s : m_Semaphore)
            s = device.createSemaphore(sCreateInfo);
#endif

#ifdef PROFILING
        vk::PhysicalDevice physDv = vulkanContext.GetPhysicalDevices()->GetVulkanDevice();
        VulkanInstance& instance = *std::reinterpret_pointer_cast<VulkanInstance>(vulkanContext.renderInstance).get();
        VulkanDevice& vulkanDevice = *std::reinterpret_pointer_cast<VulkanDevice>(vulkanContext.rhiDevice).get();

        /*
    tracyContext = tracy::CreateVkContext(physDv, device,
        vulkanDevice.GetPFN_vkResetQueryPoolEXT(),
        instance.GetPFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(),
        vulkanDevice.GetPFN_vkGetCalibratedTimestampsEXT());*/

#endif
    }
   

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
    assert(m_BufferType == PC_CORE::CommandList::BufferType::Primary);
    assert(_other);

    if (m_BufferType != PC_CORE::CommandList::BufferType::Primary)
    {
        PC_LOGCRITICAL("Only Primary Buffer can merge secondary buffer");
        return;
    }

    m_MergedCommandBufferBuffer.clear();
    m_MergedCommandBufferBuffer.reserve(_count);
    for (size_t i = 0; i < _count; i++)
    {
        const VulkanCommandList& otherVk = *reinterpret_cast<VulkanCommandList*>(_other);
        if (otherVk.GetBufferType() != PC_CORE::CommandList::BufferType::Secondary)
        {
            PC_LOGCRITICAL("Only Secondary can be merged");
            return;
        }

        vk::CommandBuffer CurrentOtherCmdBuffer = otherVk.m_CommandBuffer[m_Rhi.GetFrameIndex()];
        assert(m_CommandBuffer[m_Rhi.GetFrameIndex()] != CurrentOtherCmdBuffer);
        m_MergedCommandBufferBuffer.emplace_back(CurrentOtherCmdBuffer);
    }
    assert(m_MergedCommandBufferBuffer.size() == _count);
    m_CommandBuffer[m_Rhi.GetFrameIndex()].executeCommands(static_cast<uint32_t>(m_MergedCommandBufferBuffer.size()), m_MergedCommandBufferBuffer.data());
}

void Vulkan::VulkanCommandList::BeginRecordCommands()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    CommandList::BeginRecordCommands();
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
    CommandList::EndRecordCommands();

    m_CommandBuffer[m_Rhi.GetFrameIndex()].end();
}

void Vulkan::VulkanCommandList::BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    CommandList::BeginRenderPass(_BeginRenderPassInfo);
    m_RecordState.RecordRenderPassType.emplace(RecordRenderPassType::Graphic);

    const VulkanFrameBuffer& frameBuffer = *reinterpret_cast<const VulkanFrameBuffer*>(
        _BeginRenderPassInfo.FrameBuffer);
    const VulkanRenderPass& renderPass = *reinterpret_cast<const VulkanRenderPass*>(
        _BeginRenderPassInfo.RenderPass);


    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassInfo.renderPass = renderPass.GetVulkanRenderPass();
    renderPassInfo.framebuffer = frameBuffer.GetVkFramebuffer();
    renderPassInfo.renderArea.offset = vk::Offset2D{
        _BeginRenderPassInfo.RenderOffSet.x, _BeginRenderPassInfo.RenderOffSet.y
    };
    renderPassInfo.renderArea.extent = vk::Extent2D{_BeginRenderPassInfo.Extent.x, _BeginRenderPassInfo.Extent.y};

    assert((renderPassInfo.renderArea.offset.x + renderPassInfo.renderArea.extent.width) <= frameBuffer.GetWidth());
    assert((renderPassInfo.renderArea.offset.y + renderPassInfo.renderArea.extent.height) <= frameBuffer.GetHeight());


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
                _BeginRenderPassInfo.ClearColor[i][0],
                _BeginRenderPassInfo.ClearColor[i][1],
                _BeginRenderPassInfo.ClearColor[i][2],
                _BeginRenderPassInfo.ClearColor[i][3]
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
                _BeginRenderPassInfo.ClearColor[i][0],
                _BeginRenderPassInfo.ClearColor[i][1],
                _BeginRenderPassInfo.ClearColor[i][2],
                _BeginRenderPassInfo.ClearColor[i][3]
            });
        }
    }
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearCount);
    renderPassInfo.pClearValues = clearValues.data();

    m_CommandBuffer[m_Rhi.GetFrameIndex()].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void Vulkan::VulkanCommandList::BeginComputePasss()
{
    CommandList::BeginComputePasss();
    m_RecordState.RecordRenderPassType.emplace(RecordRenderPassType::Compute);
}

void Vulkan::VulkanCommandList::NextSubPass()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    if (!IsInRenderPass(RecordRenderPassType::Graphic))
    {
        PC_LOGERROR("NextSubPass was call but Commandlist was not in graphic record state, CommandList Name : {}", m_Name);
        return;
    }

    m_CommandBuffer[m_Rhi.GetFrameIndex()].nextSubpass(vk::SubpassContents::eInline);
}

void Vulkan::VulkanCommandList::EndRenderPass()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    if (m_RecordState.RecordRenderPassType)
    {
        switch (*m_RecordState.RecordRenderPassType)
        {
        case RecordRenderPassType::Graphic:
            m_CommandBuffer[m_Rhi.GetFrameIndex()].endRenderPass();
            break;
        default:
            break;
        }
    }
    m_RecordState.RecordRenderPassType.reset();
}

void Vulkan::VulkanCommandList::BindDescriptorSet(const PC_CORE::RhiDescriptorSet*
    _DescriptorSet, size_t _FirstSet, std::optional<size_t> _DynamicOffset)
{
    BindDescriptorSets(std::span<const PC_CORE::RhiDescriptorSet*>(&_DescriptorSet, 1), 
        _FirstSet, 
        _DynamicOffset ? std::span<size_t>(&_DynamicOffset.value(), 1ull) : std::span<size_t>());
}

void Vulkan::VulkanCommandList::BindDescriptorSets(
    const std::span<const PC_CORE::RhiDescriptorSet*>& _DescriptorSets,
    size_t _FirstSet,
    const std::span<const size_t>& dynamicOffset)
{
    
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    m_VkDescriptorSetBuffer.clear();
    m_DynamicOffsetBuffer.clear();

    const size_t currentFrame = m_Rhi.GetFrameIndex();
    m_VkDescriptorSetBuffer.reserve(_DescriptorSets.size());
    m_DynamicOffsetBuffer.reserve(dynamicOffset.size());

    for (size_t i = 0; i < _DescriptorSets.size(); i++)
    {
        const VulkanDescriptorSet& vulkanDescriptorSets = reinterpret_cast<const VulkanDescriptorSet&>(*_DescriptorSets[i]);
        m_VkDescriptorSetBuffer.emplace_back(vulkanDescriptorSets.GetVkDescriptorSet(currentFrame));
    }

    for (size_t i = 0; i < dynamicOffset.size(); i++)
        m_DynamicOffsetBuffer.emplace_back(static_cast<uint32_t>(dynamicOffset[i]));

    assert(m_VkDescriptorSetBuffer.size() == _DescriptorSets.size());
    assert(m_DynamicOffsetBuffer.size() == dynamicOffset.size());

    m_CommandBuffer[currentFrame].bindDescriptorSets(GetCurrentRecordPipelineBindPoint(),
        GetCurrentPipelineLayout(),
        static_cast<uint32_t>(_FirstSet),
        static_cast<uint32_t>(m_VkDescriptorSetBuffer.size()),
        m_VkDescriptorSetBuffer.data(),
        static_cast<uint32_t>(m_DynamicOffsetBuffer.size()),
        m_DynamicOffsetBuffer.data());
}


bool Vulkan::VulkanCommandList::BindRhiPipeline(const PC_CORE::RhiPipeline& _RhiPipeline)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const bool wasNull = !m_RecordState.lastBindProgram;

    if (m_RecordState.lastBindProgram != &_RhiPipeline)
    {
        m_RecordState.lastBindProgram = &_RhiPipeline;

        m_CommandBuffer[m_Rhi.GetFrameIndex()].bindPipeline(
            GetCurrentRecordPipelineBindPoint(),
            GetCurrentPipeline());

        return true;
    }

    return false;
}

void Vulkan::VulkanCommandList::PushConstant(RhiShaderStageTypeFlag _RhiShaderStageTypeFlag,
                                            const void* _Data,
                                            uint32_t _Offset,
                                            uint32_t _Size)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

#ifdef _DEBUG
    if (_Size > VULKAN_MAX_PUSH_CONSTANTS)
    {
        throw std::runtime_error("VULKAN_MAX_PUSH_CONSTANTS have been exceeded");
    }
#endif
    GetVulkanCommandBufferHandle().pushConstants(GetCurrentPipelineLayout(), Utils::RhiToShaderStage(_RhiShaderStageTypeFlag),
        _Offset, _Size, _Data);
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

void Vulkan::VulkanCommandList::SetPrimitiveTopology(PC_CORE::RhiGraphicPipeline::PrimitiveTopology _primitiveTopology)
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

    if (!IsInRenderPass(RecordRenderPassType::Graphic))
    {
        PC_LOGERROR("Draw was call but Commandlist was not in graphic record state, CommandList Name : {}", m_Name);
        return;
    }

    m_CommandBuffer[m_Rhi.GetFrameIndex()].draw(_vertexCount, _instanceCount, _firstVertex, _firstInstance);
}

void Vulkan::VulkanCommandList::DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex,
                                            int32_t _vertexOffset, size_t _firstInstance)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    if (!IsInRenderPass(RecordRenderPassType::Graphic))
    {
        PC_LOGERROR("DrawIndexed was call but Commandlist was not in graphic record state, CommandList Name : {}", m_Name);
        return;
    }

    assert(m_RecordState.lastDrawBuffersState.IndexBuffer != nullptr && "DrawIndexed but no index buffer binded");


    m_CommandBuffer[m_Rhi.GetFrameIndex()].drawIndexed(static_cast<uint32_t>(_indexCount),
                                                               static_cast<uint32_t>(_instanceCount),
                                                               static_cast<uint32_t>(_firstIndex), _vertexOffset,
                                                               static_cast<uint32_t>(_firstInstance));
}

void Vulkan::VulkanCommandList::Dispatch(uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    if (!IsInRenderPass(RecordRenderPassType::Compute))
    {
        PC_LOGERROR("Dispatch was call but Commandlist was not in compute record state ,CommandList Name : {}", m_Name);
        return;
    }

    m_CommandBuffer[m_Rhi.GetFrameIndex()].dispatch(_groupCountX, _groupCountY, _groupCountZ);
}

void Vulkan::VulkanCommandList::DrawMeshTask(uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    if (!IsInRenderPass(RecordRenderPassType::Graphic))
    {
        PC_LOGERROR("DrawMeshTask was call but Commandlist was not in graphic record state, CommandList Name : {}", m_Name);
        return;
    }
    // Should not be static but id does the job
    static PFN_vkCmdDrawMeshTasksEXT func = GET_VK_INSTANCE->GetPFN_vkCmdDrawMeshTasksEXT();
    assert(func && "Misssing function");

    func(m_CommandBuffer[m_Rhi.GetFrameIndex()], _groupCountX, _groupCountY, _groupCountZ);
}


void Vulkan::VulkanCommandList::BindDrawBuffers(const DrawBuffers& _DrawBuffers)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    if (!DrawBufferStateChanged(_DrawBuffers))
        return;

    const size_t frameIndex = m_Rhi.GetFrameIndex();

    struct VkVertexBufferEnty
    {
        vk::Buffer* buffers;
        vk::DeviceSize* offsets;
    } VkVertexBufferEnty;

    const uint32_t BindexVertexBufferCount = static_cast<uint32_t>(_DrawBuffers.VertexBufferBinded.size());

    VkVertexBufferEnty.buffers = reinterpret_cast<vk::Buffer*>(alloca(sizeof(vk::Buffer) * BindexVertexBufferCount));
    VkVertexBufferEnty.offsets = reinterpret_cast<vk::DeviceSize*>(alloca(sizeof(vk::DeviceSize) * BindexVertexBufferCount));

    for (size_t i = 0; i < BindexVertexBufferCount; i++)
    {
        const VulkanBuffer& vulkanBuffer = reinterpret_cast<const VulkanBuffer&>(*_DrawBuffers.VertexBufferBinded[i].first);
        const BufferAndAlloc& bufferAndAllocs = static_cast<const BufferAndAlloc&>(*vulkanBuffer.GetBufferAndAlloc(frameIndex));

        VkVertexBufferEnty.buffers[i] = bufferAndAllocs.buffer;
        VkVertexBufferEnty.offsets[i] = static_cast<vk::DeviceSize>(_DrawBuffers.VertexBufferBinded[i].second);
    }

    m_CommandBuffer[frameIndex].bindVertexBuffers(0u, BindexVertexBufferCount, VkVertexBufferEnty.buffers,
        VkVertexBufferEnty.offsets);

    if (_DrawBuffers.IndexBuffer != nullptr)
    {
        const VulkanBuffer& vulkanBuffer = reinterpret_cast<const VulkanBuffer&>(*_DrawBuffers.IndexBuffer);
        const BufferAndAlloc& bufferAndAllocs = static_cast<const BufferAndAlloc&>(*vulkanBuffer.GetBufferAndAlloc(frameIndex));

        const vk::IndexType indexType = Utils::RhiToIndexType(_DrawBuffers.IndexFormat);

        m_CommandBuffer[frameIndex].bindIndexBuffer(bufferAndAllocs.buffer, static_cast<vk::DeviceSize>(_DrawBuffers.IndexBufferOffset),
            indexType);
    }
    
}


void Vulkan::VulkanCommandList::CopyBuffer(const PC_CORE::RhiBuffer& _src, const PC_CORE::RhiBuffer& _dst,
                                           size_t _srcOffSet, size_t _dstoffset, size_t _sizeInBytes)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    
    
    const size_t frameIndex = m_Rhi.GetFrameIndex();

    const VulkanBuffer& VulkanSrcBuffer = reinterpret_cast<const VulkanBuffer&>(_src);
    const VulkanBuffer& VulkanDstBuffer = reinterpret_cast<const VulkanBuffer&>(_dst);

    const BufferAndAlloc* bufferAndAllocSrc = static_cast<const BufferAndAlloc*>(VulkanSrcBuffer.GetBufferAndAlloc(frameIndex));
    const BufferAndAlloc* bufferAndAllocDst = static_cast<const BufferAndAlloc*>(VulkanDstBuffer.GetBufferAndAlloc(frameIndex));

    vk::Buffer bufferSrc = bufferAndAllocSrc->buffer;
    vk::Buffer bufferDst = bufferAndAllocDst->buffer;

    vk::BufferCopy bufferCopy = {};
    bufferCopy.dstOffset = static_cast<uint32_t>(_dstoffset);
    bufferCopy.srcOffset = static_cast<uint32_t>(_srcOffSet);
    bufferCopy.size = static_cast<uint32_t>(_sizeInBytes);


    m_CommandBuffer[frameIndex].copyBuffer(bufferSrc, bufferDst, 1, &bufferCopy);
}

VULKAN_API void Vulkan::VulkanCommandList::Barrier(RhiResourceState _OldState, RhiResourceState _NewState,
    const std::span<const PC_CORE::ImageStateTransition>& _ImageStateTransition,
    const std::span<const PC_CORE::BufferStateTransition>& _BufferStateTransition)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const size_t frameIndex = m_Rhi.GetFrameIndex();
    const vk::AccessFlags OldAccesMask = Utils::RhiResourceStateToAccesFlag(_OldState);
    const vk::AccessFlags NewAccesMask = Utils::RhiResourceStateToAccesFlag(_NewState);
    const vk::ImageLayout OldLayout = Utils::RhiResourceStateToVulkanImageLayout(_OldState);
    const vk::ImageLayout NewMayout = Utils::RhiResourceStateToVulkanImageLayout(_NewState);


    m_VkImageBarrier.clear();
    m_VkBufferBarrier.clear();

    m_VkImageBarrier.resize(_ImageStateTransition.size());
    for (size_t i = 0; i < _ImageStateTransition.size(); i++)
    {
        PC_CORE::RhiTexture& texture = *_ImageStateTransition[i].Texture;
        VulkanTexture& vulkanTexture = static_cast<VulkanTexture&>(texture);
        TextureAndAlloc& textureAndAlloc = static_cast<TextureAndAlloc&>(*vulkanTexture.GetTextureAndAlloc());

        vk::ImageMemoryBarrier& bar = m_VkImageBarrier[i];

        bar.sType = vk::StructureType::eImageMemoryBarrier;
        bar.pNext = nullptr;
        bar.image = textureAndAlloc.Image;
        bar.srcAccessMask = OldAccesMask;
        bar.dstAccessMask = NewAccesMask;
        bar.oldLayout = OldLayout;
        bar.newLayout = NewMayout;
        bar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;


        vk::ImageSubresourceRange& ImageSubresourceRange = bar.subresourceRange;
        ImageSubresourceRange.aspectMask = vulkanTexture.VkImageAspectFlags;

        ImageSubresourceRange.baseMipLevel = _ImageStateTransition[i].FirstMipLevel;
        ImageSubresourceRange.levelCount = _ImageStateTransition[i].MipLevelsCount;

        ImageSubresourceRange.baseArrayLayer = _ImageStateTransition[i].FirstLayer;
        ImageSubresourceRange.layerCount = _ImageStateTransition[i].LayerCount;

        if (_ImageStateTransition[i].updateState)
        {
            texture.SetResourceState(_NewState);
        }
    }

    m_VkBufferBarrier.resize(_BufferStateTransition.size());

    for (size_t i = 0; i < _BufferStateTransition.size(); i++)
    {
        VulkanBuffer& VkBuffer = static_cast<VulkanBuffer&>(*_BufferStateTransition[i].Buffer);
        BufferAndAlloc& bufferAndAlloc = static_cast<BufferAndAlloc&>(*VkBuffer.GetBufferAndAlloc(frameIndex));

        m_VkBufferBarrier[i].sType = vk::StructureType::eBufferMemoryBarrier;
        m_VkBufferBarrier[i].pNext = nullptr;
        m_VkBufferBarrier[i].srcAccessMask = OldAccesMask;
        m_VkBufferBarrier[i].dstAccessMask = NewAccesMask;
        m_VkBufferBarrier[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        m_VkBufferBarrier[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        m_VkBufferBarrier[i].buffer = bufferAndAlloc.buffer;
        m_VkBufferBarrier[i].offset = _BufferStateTransition[i].Offset;
        m_VkBufferBarrier[i].size = _BufferStateTransition[i].Size == PC_CORE::WHOLE_SIZE ? VK_WHOLE_SIZE : _BufferStateTransition[i].Size;


        if (_BufferStateTransition[i].updateState)
        {
            bufferAndAlloc.ResourceState = _NewState;
        }
    }
    

    const vk::PipelineStageFlags SrcStageFlags = Utils::PipelineStageFlagsFromRhiResourceState(_OldState);
    const vk::PipelineStageFlags DstStageFlags = Utils::PipelineStageFlagsFromRhiResourceState(_NewState);

    m_CommandBuffer[frameIndex].pipelineBarrier(SrcStageFlags, DstStageFlags, {},
        0, nullptr,
        static_cast<uint32_t>(m_VkBufferBarrier.size()), m_VkBufferBarrier.data(),
        static_cast<uint32_t>(m_VkImageBarrier.size()), m_VkImageBarrier.data());

}

vk::CommandBuffer Vulkan::VulkanCommandList::GetVulkanCommandBufferHandle() const
{
    return GetVulkanCommandBufferHandle(m_Rhi.GetFrameIndex());
}

VULKAN_API vk::CommandBuffer Vulkan::VulkanCommandList::GetVulkanCommandBufferHandle(size_t _FrameIndex) const
{
    return m_CommandBuffer[_FrameIndex];
}

VULKAN_API vk::Semaphore Vulkan::VulkanCommandList::GetVkSemaphore() const
{
    return m_Semaphore[m_Rhi.GetFrameIndex()];
}

vk::PipelineBindPoint Vulkan::VulkanCommandList::GetCurrentRecordPipelineBindPoint() const
{
    return IsInRecordState() ? Utils::RhiRecordRenderPassTypeToPipelineBindPoint(*m_RecordState.RecordRenderPassType) : vk::PipelineBindPoint::eGraphics;
}

vk::PipelineLayout Vulkan::VulkanCommandList::GetCurrentPipelineLayout() const
{
    if (!IsInRecordState())
        return VK_NULL_HANDLE;

    if (!m_RecordState.lastBindProgram)
    {
        PC_LOGERROR("You are trying to use an pipiline without binding it do you forget to bind one ? Command list {}", m_Name);
        return VK_NULL_HANDLE;
    }

    switch (*m_RecordState.RecordRenderPassType)
    {
    case RecordRenderPassType::Graphic:
        return reinterpret_cast<const Vulkan::VulkanGraphicPipeline&>(*m_RecordState.lastBindProgram).GetPipelineLayout();
    case RecordRenderPassType::Compute:
        return reinterpret_cast<const Vulkan::VulkanComputePipeline&>(*m_RecordState.lastBindProgram).GetPipelineLayout();
    default:
        return VK_NULL_HANDLE;
    }
}

vk::Pipeline Vulkan::VulkanCommandList::GetCurrentPipeline() const
{
    if (!IsInRecordState())
        return VK_NULL_HANDLE;

    if (!m_RecordState.lastBindProgram)
    {
        PC_LOGERROR("You are trying to use an pipiline without binding it do you forget to bind one ? Command list {}", m_Name);
        return VK_NULL_HANDLE;
    }

    switch (*m_RecordState.RecordRenderPassType)
    {
    case RecordRenderPassType::Graphic:
        return reinterpret_cast<const Vulkan::VulkanGraphicPipeline&>(*m_RecordState.lastBindProgram).GetPipeline();
    case RecordRenderPassType::Compute:
        return reinterpret_cast<const Vulkan::VulkanComputePipeline&>(*m_RecordState.lastBindProgram).GetPipeline();
    default:
        return VK_NULL_HANDLE;
    }
}

bool Vulkan::VulkanCommandList::IsInRecordState() const
{
    if (!m_RecordState.RecordRenderPassType)
    {
        PC_LOGERROR("You are trying to use an command buffer that is not bound in an render pass Command list {} \n do you forgot to call begin render pass / compute / raytracing", m_Name);
        return false;
    }

    return true;
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
