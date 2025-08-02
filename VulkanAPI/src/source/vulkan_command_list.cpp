#include "perf_region.hpp"

#include "vulkan_command_list.hpp"

#include "utils/rhi_vulkan_parser.hpp"
#include "vulkan_context.hpp"
#include "vulkan_frame_buffer.hpp"
#include "vulkan_render_pass.hpp"
#include "buffer/vulkan_buffer.hpp"
#include "low_renderer/rhi.hpp"
#include "resources/vulkan_descriptor_sets.hpp"
#include "resources/vulkan_shader_program.hpp"
#include "vulkan_fence.hpp"
#include <texture/vulkan_texture.hpp>




Vulkan::VulkanCommandList::VulkanCommandList(const PC_CORE::CommandListCreateInfo& _commandListCreateInfo)
: CommandList(_commandListCreateInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    VulkanContext& vulkanContext = VulkanContext::GetContext();
    vk::Device device = GET_VK_DEVICE->GetDevice();

    vk::CommandPool commandPool = VK_NULL_HANDLE;
    
    switch (m_CommandPoolFamily)
    {
    case PC_CORE::CommandPoolFamily::Graphics:
        commandPool = vulkanContext.commandPool;
        break;
    case PC_CORE::CommandPoolFamily::Compute:
        break;
    case PC_CORE::CommandPoolFamily::Count:
        break;
    default: ;
    }

    m_CommandBufferType = _commandListCreateInfo.commandBufferType;
    
    if (commandPool == VK_NULL_HANDLE)
    {
        // TO DO GET CALL FOR LOGGING
        return;
    }
    
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = m_CommandBufferType == PC_CORE::CommandBufferType::Primary ?
        vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary;

    commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffer.size());

   std::vector<vk::CommandBuffer> vec = device.allocateCommandBuffers(commandBufferAllocateInfo);

    for (uint32_t i = 0; i < m_CommandBuffer.size(); i++)
        m_CommandBuffer[i] = vec[i];

    vk::SemaphoreCreateInfo sCreateInfo;
    sCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

    for (auto& s : m_Semaphore)
        s = device.createSemaphore(sCreateInfo);
    
#ifdef PROFILING
    vk::PhysicalDevice physDv = vulkanContext.GetPhysicalDevices()->GetVulkanDevice();
    VulkanInstance& instance = *std::reinterpret_pointer_cast<VulkanInstance>(vulkanContext.renderInstance).get();
    VulkanDevice& vulkanDevice = *GET_VK_DEVICE.get();

    tracyContext = tracy::CreateVkContext(physDv, device,
        vulkanDevice.GetPFN_vkResetQueryPoolEXT(),
        instance.GetPFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(),
        vulkanDevice.GetPFN_vkGetCalibratedTimestampsEXT());
       
#endif
}

Vulkan::VulkanCommandList::~VulkanCommandList()
{
    vk::Device device = GET_VK_DEVICE->GetDevice();

    for (auto& s : m_Semaphore)
        device.destroySemaphore(s);

#ifdef PROFILING
    tracy::DestroyVkContext(tracyContext);
    tracyContext = nullptr;
#endif
}

void Vulkan::VulkanCommandList::Reset()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].reset();
}

void Vulkan::VulkanCommandList::MergeCommands(CommandList* _other, size_t _count)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    assert(_count != 0);
    assert(this != _other);
    assert(m_CommandBufferType == PC_CORE::CommandBufferType::Primary);

    vk::CommandBuffer* commandBuffers = reinterpret_cast<vk::CommandBuffer*>(_malloca(sizeof(vk::CommandBuffer) * _count));

    for (size_t i = 0; i < _count; i++)
    {
        VulkanCommandList* otherVk = reinterpret_cast<VulkanCommandList*>(_other);
        commandBuffers[i] = otherVk->m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()];
    }

    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].executeCommands(_count, commandBuffers);
}

void Vulkan::VulkanCommandList::BeginRecordCommands()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const uint32_t frameIndex = PC_CORE::Rhi::GetFrameIndex();

    vk::CommandBufferInheritanceInfo inheritanceInfo;
    if (m_CommandBufferType == PC_CORE::CommandBufferType::Secondary)
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
    commandBufferBeginInfo.pInheritanceInfo = m_CommandBufferType == PC_CORE::CommandBufferType::Secondary ? &inheritanceInfo : nullptr; // Optional

    m_CommandBuffer[frameIndex].begin(commandBufferBeginInfo);
}

void Vulkan::VulkanCommandList::EndRecordCommands()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

     m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].end();
}

void Vulkan::VulkanCommandList::BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    std::shared_ptr<const VulkanFrameBuffer> frameBuffer = std::reinterpret_pointer_cast<const  VulkanFrameBuffer>(_BeginRenderPassInfo.frameBuffer);
    std::shared_ptr<const  VulkanRenderPass> renderPass = std::reinterpret_pointer_cast<const  VulkanRenderPass>(_BeginRenderPassInfo.renderPass);

    
    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = vk::StructureType::eRenderPassBeginInfo;
    renderPassInfo.renderPass = renderPass->GetVulkanRenderPass();
    renderPassInfo.framebuffer = frameBuffer->GetFramebuffer();
    renderPassInfo.renderArea.offset = vk::Offset2D{_BeginRenderPassInfo.renderOffSet.x, _BeginRenderPassInfo.renderOffSet.y};
    renderPassInfo.renderArea.extent = vk::Extent2D{_BeginRenderPassInfo.extent.x, _BeginRenderPassInfo.extent.y};

    constexpr size_t MaxClearValues = 10;
    size_t clearCount = 0;
    std::array<vk::ClearValue, MaxClearValues> clearValues;
    assert(_BeginRenderPassInfo.clearValueCount <  clearValues.size() && "clearValues.size() should be graeter than _BeginRenderPassInfo.clearValueCount");
    
    if ((_BeginRenderPassInfo.clearValueFlags & PC_CORE::ClearValueFlags::ClearValueColor) &&
    (_BeginRenderPassInfo.clearValueFlags & PC_CORE::ClearValueFlags::ClearValueDepth))
    {
        // Clear + Depht
        clearCount = _BeginRenderPassInfo.clearValueCount + 1;
        for (size_t i = 0; i < _BeginRenderPassInfo.clearValueCount; i++)
        {
            clearValues[0].color.setFloat32({
           _BeginRenderPassInfo.clearColor[i].x,
           _BeginRenderPassInfo.clearColor[i].y,
           _BeginRenderPassInfo.clearColor[i].z,
           _BeginRenderPassInfo.clearColor[i].w});
        }
       
        clearValues[clearCount - 1].depthStencil.setDepth(_BeginRenderPassInfo.clearDepth);
    }
    else if (_BeginRenderPassInfo.clearValueFlags & PC_CORE::ClearValueFlags::ClearValueDepth)
    {
        clearValues[0].depthStencil.setDepth(_BeginRenderPassInfo.clearDepth);
        clearCount = 1;

    }
    else if (_BeginRenderPassInfo.clearValueFlags & PC_CORE::ClearValueFlags::ClearValueColor)
    {
        clearCount = _BeginRenderPassInfo.clearValueCount;
        for (size_t i = 0; i < _BeginRenderPassInfo.clearValueCount; i++)
        {
            clearValues[0].color.setFloat32({
           _BeginRenderPassInfo.clearColor[i].x,
           _BeginRenderPassInfo.clearColor[i].y,
           _BeginRenderPassInfo.clearColor[i].z,
           _BeginRenderPassInfo.clearColor[i].w});
        }
    }
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearCount);
    renderPassInfo.pClearValues = clearValues.data();

    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void Vulkan::VulkanCommandList::NextSubPass()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].nextSubpass(vk::SubpassContents::eInline);
}

void Vulkan::VulkanCommandList::EndRenderPass()
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].endRenderPass();
}

void Vulkan::VulkanCommandList::BindDescriptorSet(const PC_CORE::ShaderProgram* _shaderProgram,
    const PC_CORE::ShaderProgramDescriptorSets* _shaderProgramDescriptorSets, size_t _firstSet,
    size_t _descriptorSetCount)
{

    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const size_t currentFrame = PC_CORE::Rhi::GetFrameIndex();
    
    const VulkanShaderProgram* shaderProgram = reinterpret_cast<const VulkanShaderProgram*>(_shaderProgram->GetRhiHandle().get());

    const VulkanDescriptorSets* vulkanDescriptorSets = reinterpret_cast<const VulkanDescriptorSets*>(_shaderProgramDescriptorSets);
    const std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT>& descriptorHandles = *static_cast<const std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT>*>(vulkanDescriptorSets->GetNativeHandle());

    vk::DescriptorSet currentDescriptorSet = descriptorHandles[currentFrame];

    m_CommandBuffer[currentFrame].bindDescriptorSets(shaderProgram->GetPipelineBindPoint(),
        shaderProgram->GetPipelineLayout(), static_cast<uint32_t>(_firstSet), _descriptorSetCount,
        &currentDescriptorSet,
        0, nullptr);
}

void Vulkan::VulkanCommandList::BindProgram(const PC_CORE::ShaderProgram* _shaderProgramm)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const VulkanShaderProgram* vshadeProgram = reinterpret_cast<const VulkanShaderProgram*>(_shaderProgramm->GetRhiHandle().get());
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].bindPipeline(vshadeProgram->GetPipelineBindPoint(), vshadeProgram->GetPipeline());
}

void Vulkan::VulkanCommandList::PushConstant(const PC_CORE::ShaderProgram* _shaderProgram, const std::string& _pushConstantKey, const void* _data, const size_t _size)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    const VulkanShaderProgram* vshadeProgram = reinterpret_cast<const VulkanShaderProgram*>(_shaderProgram->GetRhiHandle().get()); 

    vshadeProgram->PushConstant(GetVkHandle(), _pushConstantKey, _data, _size);
}

void Vulkan::VulkanCommandList::SetViewPort(const PC_CORE::ViewportInfo& _viewPort)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    vk::Viewport viewport{};
    viewport.x = _viewPort.transform.x;
    viewport.y = _viewPort.transform.y;
    viewport.width = static_cast<float>(_viewPort.size.x);
    viewport.height = static_cast<float>(_viewPort.size.y);
    viewport.minDepth = _viewPort.minDepth;
    viewport.maxDepth = _viewPort.maxDepth;
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{_viewPort.scissorsOff.x, _viewPort.scissorsOff.y};
    scissor.extent = vk::Extent2D{_viewPort.scissorsextent.x, _viewPort.scissorsextent.y};
    
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].setScissor(0, 1, &scissor);
}

void Vulkan::VulkanCommandList::SetPrimitiveTopology(PC_CORE::PrimitiveTopology _primitiveTopology)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    vk::PrimitiveTopology topology = Utils::RhiPrimitiveTopology(_primitiveTopology);
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].setPrimitiveTopology(topology);
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

    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].setColorBlendEquationEXT(_firstAttachement, _attachementCount, &blendEq);*/
}

void Vulkan::VulkanCommandList::SetLineWidth(float _widht)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].setLineWidth(_widht);
}

void Vulkan::VulkanCommandList::Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex,
                                     uint32_t _firstInstance)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].draw(_vertexCount, _instanceCount, _firstVertex, _firstInstance);
}

void Vulkan::VulkanCommandList::DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex,
    int32_t _vertexOffset, size_t _firstInstance)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].drawIndexed(static_cast<uint32_t>(_indexCount), static_cast<uint32_t>(_instanceCount), static_cast<uint32_t>(_firstIndex), _vertexOffset, static_cast<uint32_t>(_firstInstance));
}

void Vulkan::VulkanCommandList::Dispatch(uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);


    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].dispatch(_groupCountX, _groupCountY, _groupCountZ);
}

void Vulkan::VulkanCommandList::BindVertexBuffer(const PC_CORE::RhiVertexBuffer& _vertexBuffer, uint32_t _firstBinding,
                                                 uint32_t _bindingCount)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const size_t frameIndex = PC_CORE::Rhi::GetFrameIndex(); 
    
    const  std::vector<BufferAndAlloc>* bufferAndAllocs = static_cast<const std::vector<BufferAndAlloc>*>(_vertexBuffer.GetNativeHandle());
    
    vk::DeviceSize offsets[] = {0};
    m_CommandBuffer[frameIndex].bindVertexBuffers(_firstBinding, _bindingCount, &bufferAndAllocs->at(frameIndex).buffer, offsets);
}

void Vulkan::VulkanCommandList::BindIndexBuffer(const PC_CORE::RhiIndexBuffer& _indexBuffer, size_t _offset)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    const size_t frameIndex = PC_CORE::Rhi::GetFrameIndex(); 
    const std::vector<BufferAndAlloc>* bufferAndAllocs = static_cast<const std::vector<BufferAndAlloc>*>(_indexBuffer.GetNativeHandle());
    const vk::IndexType indexType = Vulkan::Utils::RhiToIndexType(_indexBuffer.GetIndexFormat());
    
    m_CommandBuffer[frameIndex].bindIndexBuffer(bufferAndAllocs->at(frameIndex).buffer, static_cast<uint32_t>(_offset) , indexType);
}

void Vulkan::VulkanCommandList::CopyBuffer(const PC_CORE::RhiBuffer& _src, const PC_CORE::RhiBuffer& _dst, size_t _srcOffSet, size_t _dstoffset, size_t _sizeInBytes)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    assert(
        _src.GetMemoryVisibility() == PC_CORE::MemoryLocalisation::CPU_To_GPU ||
        _src.GetMemoryVisibility() == PC_CORE::MemoryLocalisation::CPU_Only
    );

    assert(_dst.GetMemoryVisibility() == PC_CORE::MemoryLocalisation::GPU_Only);


    const size_t frameIndex = PC_CORE::Rhi::GetFrameIndex();

    const std::vector<BufferAndAlloc>* bufferAndAllocSrc = reinterpret_cast<const std::vector<BufferAndAlloc>*>(_src.GetNativeHandle());
    const std::vector<BufferAndAlloc>* bufferAndAllocDst = reinterpret_cast<const std::vector<BufferAndAlloc>*>(_dst.GetNativeHandle());

    vk::Buffer bufferSrc = bufferAndAllocSrc->at(frameIndex).buffer;
    vk::Buffer bufferDst = bufferAndAllocDst->at(frameIndex).buffer;

    vk::BufferCopy bufferCopy = {};
    bufferCopy.dstOffset = static_cast<uint32_t>(_dstoffset);
    bufferCopy.srcOffset = static_cast<uint32_t>(_srcOffSet);
    bufferCopy.size = static_cast<uint32_t>(_sizeInBytes);


    m_CommandBuffer[frameIndex].copyBuffer(bufferSrc, bufferDst, 1, &bufferCopy);

    vk::BufferMemoryBarrier bufferBarrier{};
    bufferBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    bufferBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead; // or eShaderWrite if the shader writes to it
    bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferBarrier.buffer = bufferDst;
    bufferBarrier.offset = 0;
    bufferBarrier.size = VK_WHOLE_SIZE;

}

void Vulkan::VulkanCommandList::Barrier(PC_CORE::GpuPipelineStageFlagBits srcStageMask, PC_CORE::GpuPipelineStageFlagBits dstStageMask, 
    const PC_CORE::MemoryBarrier* _memoryBarrier, size_t _memoryBarrierCount, 
    const PC_CORE::BufferMemoryBarrier* _buffermemoryBarrier, size_t _bufferMemoryBarrierCount,
    const PC_CORE::ImageMemoryBarrier* _imageMemoryBarrier, size_t _imageMemoryBarrierCount)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);
    
    const size_t frameIndex = PC_CORE::Rhi::GetFrameIndex();

    const vk::PipelineStageFlags srcStageFlag = Vulkan::Utils::RhiPipelineStageToVulkan(srcStageMask);
    const vk::PipelineStageFlags dstStageFlag = Vulkan::Utils::RhiPipelineStageToVulkan(dstStageMask);

    // Memory Barrier
    std::vector<vk::MemoryBarrier> vkMemoryBarriers;
    vkMemoryBarriers.resize(_memoryBarrierCount);
    for (size_t i = 0; i < _memoryBarrierCount; i++)
    {
        vkMemoryBarriers[i].sType = vk::StructureType::eMemoryBarrier;
        vkMemoryBarriers[i].pNext = nullptr;
        vkMemoryBarriers[i].srcAccessMask = Vulkan::Utils::RhiAccessFlagToVulkan(_memoryBarrier[i].srcAccessMask);
        vkMemoryBarriers[i].dstAccessMask = Vulkan::Utils::RhiAccessFlagToVulkan(_memoryBarrier[i].dstAccessMask);
    }
    // BufferMemoryBarrier
    std::vector<vk::BufferMemoryBarrier> vkBufferBarrier;
    vkBufferBarrier.resize(_bufferMemoryBarrierCount);

    for (size_t i = 0; i < _bufferMemoryBarrierCount; i++)
    {
        VulkanBuffer* vkBuffer = static_cast<VulkanBuffer*>(_buffermemoryBarrier[i].buffer->GetNativeHandle());

		vkBufferBarrier[i].sType = vk::StructureType::eBufferMemoryBarrier;
		vkBufferBarrier[i].pNext = nullptr;
		vkBufferBarrier[i].srcAccessMask = Vulkan::Utils::RhiAccessFlagToVulkan(_buffermemoryBarrier[i].srcAccessMask);
		vkBufferBarrier[i].dstAccessMask = Vulkan::Utils::RhiAccessFlagToVulkan(_buffermemoryBarrier[i].dstAccessMask);
		vkBufferBarrier[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		vkBufferBarrier[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		vkBufferBarrier[i].buffer = vkBuffer->bufferAndAlloc[frameIndex].buffer;
		vkBufferBarrier[i].offset = _buffermemoryBarrier[i].offset;
		vkBufferBarrier[i].size = _buffermemoryBarrier[i].size;       
    }

    // ImageMemoryBarrier
    std::vector<vk::ImageMemoryBarrier> vkImageBarrier;
    vkImageBarrier.resize(_imageMemoryBarrierCount);
    
    for (size_t i = 0; i < _imageMemoryBarrierCount; i++)
    {
        PC_CORE::RhiTexture* texture = _imageMemoryBarrier[i].texture;
        VulkanTexture* textureAndAlloc = reinterpret_cast<VulkanTexture*>(texture->GetNativeHandle());

        vk::ImageMemoryBarrier& bar = vkImageBarrier[i];

        bar.sType = vk::StructureType::eImageMemoryBarrier;
        bar.pNext = nullptr;
        bar.image = textureAndAlloc->textureAndAlloc[frameIndex].image;
        bar.srcAccessMask = Vulkan::Utils::RhiAccessFlagToVulkan(_imageMemoryBarrier[i].srcAccessMask);
        bar.dstAccessMask = Vulkan::Utils::RhiAccessFlagToVulkan(_imageMemoryBarrier[i].dstAccessMask);
        bar.oldLayout = Vulkan::Utils::RhiImageStateToVulkanImageLayout(_imageMemoryBarrier[i].currentState);
        bar.newLayout = Vulkan::Utils::RhiImageStateToVulkanImageLayout(_imageMemoryBarrier[i].newState);
        bar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;


        vk::ImageSubresourceRange& ImageSubresourceRange = bar.subresourceRange;
        ImageSubresourceRange.aspectMask = Vulkan::Utils::RhiTextureUsageToImageAspectFlagFlags(texture->GetTextureUsage());
        ImageSubresourceRange.baseArrayLayer = 0;
        ImageSubresourceRange.layerCount = texture->GetLayerCount();
        ImageSubresourceRange.baseMipLevel = 0;
        ImageSubresourceRange.baseMipLevel = texture->GetMipLevelCount();
    }

    m_CommandBuffer[frameIndex].pipelineBarrier(srcStageFlag, dstStageFlag, {},
        static_cast<uint32_t>(vkMemoryBarriers.size()), vkMemoryBarriers.data(),
        static_cast<uint32_t>(vkBufferBarrier.size()),vkBufferBarrier.data(),
        0, nullptr);
}

void Vulkan::VulkanCommandList::Flush(PC_CORE::FlushCommandMethod _flushCommandMethod, PC_CORE::GpuPipelineStageFlagBits _waitGpuPipelineStageFlag)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Rhi);

    assert(_flushCommandMethod == PC_CORE::FlushCommandMethod::Sync);

    VulkanContext& vkContext = VulkanContext::GetContext();
    const size_t frameIndex = PC_CORE::Rhi::GetFrameIndex();

    switch (m_CommandPoolFamily)
    {
    case PC_CORE::CommandPoolFamily::Graphics:
        vkContext.flushedCommands.emplace_back(FlushCommand{ m_CommandBuffer[frameIndex], m_Semaphore[frameIndex], _waitGpuPipelineStageFlag });
        break;
    case PC_CORE::CommandPoolFamily::Compute: // not implemented yet
    case PC_CORE::CommandPoolFamily::Count:
        assert(false);
        break;
    default:;
    }

}

vk::CommandBuffer Vulkan::VulkanCommandList::GetVkHandle() const
{
    return m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()];
}


void Vulkan::VulkanCommandList::BeginDebugLabel(const char* _debugLabel, const std::array<float, 4>& _color)
{

#ifdef  DEBUG_GPU_ON
    VkDebugUtilsLabelEXT markerInfo = {};
    markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    markerInfo.pLabelName = _debugLabel;

    std::memcpy(&markerInfo.color[0], _color.data(), sizeof(float) * _color.size());
    static auto begindDebugLabelPtrFunc = std::reinterpret_pointer_cast<Vulkan::VulkanInstance>(PC_CORE::Rhi::GetRhiContext()->renderInstance)->GetPFN_vkCmdBeginDebugUtilsLabelEXT();
    begindDebugLabelPtrFunc(m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()], &markerInfo);
#endif
 
}

void Vulkan::VulkanCommandList::EndDebugLabel()
{
#ifdef  DEBUG_GPU_ON
    static auto endDebugLabelPtrFunc = std::reinterpret_pointer_cast<Vulkan::VulkanInstance>(PC_CORE::Rhi::GetRhiContext()->renderInstance)->GetPFN_vkCmdEndDebugUtilsLabelEXT();
    endDebugLabelPtrFunc(m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()]);
#endif
}
