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



Vulkan::VulkanCommandList::VulkanCommandList(const PC_CORE::CommandListCreateInfo& _commandListCreateInfo)
: CommandList(_commandListCreateInfo)
{
    VulkanContext& vulkanContext = VulkanContext::GetContext();
    vk::CommandPool commandPool = VK_NULL_HANDLE;
    
    switch (m_CommandPoolFamily)
    {
    case PC_CORE::CommandPoolFamily::Graphics:
        commandPool = vulkanContext.commandPool;
        m_Queue = &vulkanContext.mainQueue;
        break;
    case PC_CORE::CommandPoolFamily::Compute:
        break;
    case PC_CORE::CommandPoolFamily::Count:
        break;
    default: ;
    }
    
    if (commandPool == VK_NULL_HANDLE)
    {
        // TO DO GET CALL FOR LOGGING
        return;
    }
    
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffer.size());

   std::vector<vk::CommandBuffer> vec = vulkanContext.GetDevice()->GetDevice().allocateCommandBuffers(commandBufferAllocateInfo);

    for (uint32_t i = 0; i < m_CommandBuffer.size(); i++)
    {
        m_CommandBuffer[i] = vec[i];
    }
}

Vulkan::VulkanCommandList::~VulkanCommandList()
{
    
}

void Vulkan::VulkanCommandList::Reset()
{
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].reset();
}

void Vulkan::VulkanCommandList::BeginRecordCommands()
{
    const uint32_t frameIndex = PC_CORE::Rhi::GetFrameIndex();
    
    vk::CommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
    commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits(); // Optional
    commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

    m_CommandBuffer[frameIndex].begin(commandBufferBeginInfo);

}

void Vulkan::VulkanCommandList::EndRecordCommands()
{
     m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].end();
}

void Vulkan::VulkanCommandList::BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo)
{
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

void Vulkan::VulkanCommandList::EndRenderPass()
{
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].endRenderPass();
}

void Vulkan::VulkanCommandList::BindDescriptorSet(const PC_CORE::ShaderProgram* _shaderProgram,
    const PC_CORE::ShaderProgramDescriptorSets* _shaderProgramDescriptorSets, size_t _firstSet,
    size_t _descriptorSetCount)
{
    const size_t currentFrame = PC_CORE::Rhi::GetFrameIndex();
    
    const VulkanShaderProgram* shaderProgram = reinterpret_cast<const VulkanShaderProgram*>(_shaderProgram->GetRhiHandle().get());
    const VulkanDescriptorSets* vulkanDescriptorSets = reinterpret_cast<const VulkanDescriptorSets*>(_shaderProgramDescriptorSets);
    
    m_CommandBuffer[currentFrame].bindDescriptorSets(shaderProgram->GetPipelineBindPoint(),
        shaderProgram->GetPipelineLayout(), static_cast<uint32_t>(_firstSet), _descriptorSetCount,
        vulkanDescriptorSets->descriptorSets.data() + currentFrame,
        0, nullptr);
}

void Vulkan::VulkanCommandList::BindProgram(const PC_CORE::ShaderProgram* _shaderProgramm)
{
    const VulkanShaderProgram* vshadeProgram = reinterpret_cast<const VulkanShaderProgram*>(_shaderProgramm->GetRhiHandle().get());
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].bindPipeline(vshadeProgram->GetPipelineBindPoint(), vshadeProgram->GetPipeline());
}

void Vulkan::VulkanCommandList::PushConstant(const PC_CORE::ShaderProgram* _shaderProgram, const std::string& _pushConstantKey, void* _data, const size_t _size)
{
    const VulkanShaderProgram* vshadeProgram = reinterpret_cast<const VulkanShaderProgram*>(_shaderProgram->GetRhiHandle().get()); 

    vshadeProgram->PushConstant(GetHandle(), _pushConstantKey, _data, _size);
}

void Vulkan::VulkanCommandList::SetViewPort(const PC_CORE::ViewportInfo& _viewPort)
{
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
    vk::PrimitiveTopology topology = Utils::RhiPrimitiveTopology(_primitiveTopology);
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].setPrimitiveTopology(topology);
}

void Vulkan::VulkanCommandList::SetLineWidth(float _widht)
{
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].setLineWidth(_widht);
}

void Vulkan::VulkanCommandList::Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex,
                                     uint32_t _firstInstance)
{
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].draw(_vertexCount, _instanceCount, _firstVertex, _firstInstance);
}

void Vulkan::VulkanCommandList::DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex,
    int32_t _vertexOffset, size_t _firstInstance)
{
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].drawIndexed(static_cast<uint32_t>(_indexCount), static_cast<uint32_t>(_instanceCount), static_cast<uint32_t>(_firstIndex), _vertexOffset, static_cast<uint32_t>(_firstInstance));
}

void Vulkan::VulkanCommandList::BindVertexBuffer(const PC_CORE::RhiVertexBuffer& _vertexBuffer, uint32_t _firstBinding,
                                                 uint32_t _bindingCount)
{
    const size_t frameIndex = PC_CORE::Rhi::GetFrameIndex(); 
    
    const  std::vector<BufferAndAlloc>* bufferAndAllocs = static_cast<const std::vector<BufferAndAlloc>*>(_vertexBuffer.GetNativeHandle());
    
    vk::DeviceSize offsets[] = {0};
    m_CommandBuffer[frameIndex].bindVertexBuffers(_firstBinding, _bindingCount, &bufferAndAllocs->at(frameIndex).buffer, offsets);
}

void Vulkan::VulkanCommandList::BindIndexBuffer(const PC_CORE::RhiIndexBuffer& _indexBuffer, size_t _offset)
{
    const size_t frameIndex = PC_CORE::Rhi::GetFrameIndex(); 
    const  std::vector<BufferAndAlloc>* bufferAndAllocs = static_cast<const std::vector<BufferAndAlloc>*>(_indexBuffer.GetNativeHandle());
    const vk::IndexType indexType = Vulkan::Utils::RhiToIndexType(_indexBuffer.GetIndexFormat());
    
    m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()].bindIndexBuffer(bufferAndAllocs->at(frameIndex).buffer, static_cast<uint32_t>(_offset) , indexType);
}

vk::CommandBuffer Vulkan::VulkanCommandList::GetHandle() const
{
    return m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()];
}

const vk::Queue* Vulkan::VulkanCommandList::GetQueue() const
{
    return m_Queue;
}

void Vulkan::VulkanCommandList::BeginDebugLabel(const char* _debugLabel, const std::array<float, 4>& _color)
{

#ifdef  WITH_EDITOR || PROFING
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
#ifdef  WITH_EDITOR || PROFING
    static auto endDebugLabelPtrFunc = std::reinterpret_pointer_cast<Vulkan::VulkanInstance>(PC_CORE::Rhi::GetRhiContext()->renderInstance)->GetPFN_vkCmdEndDebugUtilsLabelEXT();
    endDebugLabelPtrFunc(m_CommandBuffer[PC_CORE::Rhi::GetFrameIndex()]);
#endif
}
