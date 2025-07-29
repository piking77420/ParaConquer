#pragma once

#include <array>
#include <string>
#include <memory>
#include <functional>

#include "core_header.hpp"
#include "math/toolbox_typedef.hpp"

#include "frame_buffer.hpp"
#include "rhi_index_buffer.hpp"
#include "rhi_render_pass.hpp"
#include "rhi_vertex_buffer.hpp"


#include "rendering/shader_program.hpp"
#include "rendering/buffer/index_buffer.hpp"
#include "rendering/buffer/vertex_buffer.hpp"
#include "rhi_fence.hpp"

BEGIN_PCCORE
    class FrameBuffer;

    enum class CommandPoolFamily
{
    Graphics,
    Compute,
    Count
};


enum ClearValueFlags : uint32_t
{
    ClearValueNone    = 0,
    ClearValueColor   = 1 << 0, 
    ClearValueDepth   = 1 << 1, 
    ClearValueStencil = 1 << 2, 
    ClearValueCount   = 1 << 3  
};


struct BeginRenderPassInfo
{
    std::shared_ptr<PC_CORE::RhiRenderPass> renderPass;
    std::shared_ptr<PC_CORE::FrameBuffer> frameBuffer;
    Tbx::Vector2i renderOffSet;
    Tbx::Vector2ui extent;

    ClearValueFlags clearValueFlags;
    Tbx::Vector4f* clearColor;
    size_t clearValueCount;
    float clearDepth = 0.f;
    float clearStencil = 0.f;
    
};

struct ViewportInfo
{
    Tbx::Vector2f transform;
    Tbx::Vector2f size;
    float minDepth;
    float maxDepth;

    Tbx::Vector2i scissorsOff;
    Tbx::Vector2ui scissorsextent;
};

enum struct CommandBufferType
{
    Primary,
    Secondary
};

struct CommandListCreateInfo
{
    CommandPoolFamily commandPoolFamily;
    CommandBufferType commandBufferType;
};

struct MemoryBarrier
{
    GpuAccessFlag srcAccessMask;
    GpuAccessFlag dstAccessMask;
};

struct BufferMemoryBarrier
{
    GpuAccessFlag srcAccessMask;
    GpuAccessFlag dstAccessMask;
    RhiBuffer* buffer;
    size_t offset;
    size_t size;
};

struct ImageMemoryBarrier
{
    // TODO
    GpuAccessFlag srcAccessMask;
    GpuAccessFlag dstAccessMask;
};

enum struct FlushCommandMethod
{
    Sync, // will flush command at once at the end of tick
    Async, // send directly to gpu // TODO
};

class CommandList
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(CommandList)

    PC_CORE_API CommandList(const CommandListCreateInfo& _commandListCreateInfo);
    
    PC_CORE_API CommandList() = default;

    PC_CORE_API virtual ~CommandList() = default;

    PC_CORE_API virtual void Reset() = 0;

    PC_CORE_API virtual void MergeCommands(CommandList* _secondaries, size_t _count) = 0;

    PC_CORE_API virtual void BeginRecordCommands() = 0;

    PC_CORE_API virtual void EndRecordCommands() = 0;

    PC_CORE_API virtual void BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo) = 0;

    PC_CORE_API virtual void EndRenderPass() = 0;

    PC_CORE_API virtual void NextSubPass() = 0;
    
    PC_CORE_API virtual void BindDescriptorSet(const ShaderProgram* _shaderProgram, const ShaderProgramDescriptorSets* _shaderProgramDescriptorSets,
        size_t _firstSet, size_t _descriptorSetCount) = 0;

    PC_CORE_API virtual void BindProgram(const ShaderProgram* _shaderProgramm) = 0;

    PC_CORE_API virtual void PushConstant(const PC_CORE::ShaderProgram* _shaderProgram, const std::string& _pushConstantKey,
        const void* _data, size_t _size) = 0;

    PC_CORE_API virtual void SetViewPort(const ViewportInfo& _viewPort) = 0;

    PC_CORE_API virtual void SetPrimitiveTopology(PrimitiveTopology _primitiveTopology) = 0;

    PC_CORE_API virtual void SetBlendEquation(uint32_t _firstAttachement, uint32_t _attachementCount) = 0;

    PC_CORE_API virtual void SetLineWidth(float _widht) = 0;

    PC_CORE_API virtual void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance) = 0;

    PC_CORE_API virtual void DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex, int32_t _vertexOffset, size_t _firstInstance) = 0;

    PC_CORE_API virtual void BindVertexBuffer(const PC_CORE::RhiVertexBuffer& _vertexBuffer, uint32_t _firstBinding, uint32_t _bindingCount) = 0;

    PC_CORE_API virtual void BindIndexBuffer(const PC_CORE::RhiIndexBuffer& _indexBuffer, size_t _offset) = 0;

    PC_CORE_API virtual void CopyBuffer(const PC_CORE::RhiBuffer& _src, const PC_CORE::RhiBuffer& _dst, size_t _srcOffSet, size_t _dstoffset, size_t _sizeInBytes) = 0;

    PC_CORE_API virtual void Barrier(PC_CORE::GpuPipelineStageFlagBits srcStageMask, PC_CORE::GpuPipelineStageFlagBits dstStageMask,
        const PC_CORE::MemoryBarrier* _memoryBarrier, size_t _memoryBarrierCount,
        const PC_CORE::BufferMemoryBarrier* _buffermemoryBarrier, size_t _bufferMemoryBarrierCount,
        const PC_CORE::ImageMemoryBarrier* _imageMemoryBarrier, size_t _imageMemoryBarrierCount) = 0;

    PC_CORE_API void RecordFetchCommand(std::function<void(CommandList*)> _fectFunction);

    PC_CORE_API void ExecuteExternalCommand();

    PC_CORE_API virtual void Flush(FlushCommandMethod _flushCommandMethod, 
        PC_CORE::GpuPipelineStageFlagBits _waitGpuPipelineStageFlag) = 0;

    PC_CORE_API virtual void BeginDebugLabel(const char* _debugLabel, const std::array<float, 4>& _color) = 0;

    PC_CORE_API virtual void EndDebugLabel() = 0;


protected:
    CommandPoolFamily m_CommandPoolFamily;

    CommandBufferType m_CommandBufferType;

    std::vector<std::function<void(CommandList*)>> m_FecthCommands;
};


END_PCCORE
