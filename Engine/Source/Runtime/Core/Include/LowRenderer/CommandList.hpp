#pragma once

#include <Array>
#include <String>
#include <Memory>
#include <Functional>

#include "CoreHeader.hpp"
#include "Math/ToolboxTypedef.hpp"

#include "FrameBuffer.hpp"
#include "RhiIndexBuffer.hpp"
#include "RhiRenderPass.hpp"
#include "RhiVertexBuffer.hpp"


#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Buffer/IndexBuffer.hpp"
#include "Rendering/Buffer/VertexBuffer.hpp"
#include "RhiFence.hpp"

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
        ClearValueNone = 0,
        ClearValueColor = 1 << 0,
        ClearValueDepth = 1 << 1,
        ClearValueStencil = 1 << 2,
        ClearValueCount = 1 << 3
    };


    struct BeginRenderPassInfo
    {
        std::shared_ptr<RhiRenderPass> RenderPass;
        std::shared_ptr<FrameBuffer> FrameBuffer;
        Tbx::Vector2i RenderOffSet;
        Tbx::Vector2ui Extent;

        ClearValueFlags ClearValueFlags;
        Tbx::Vector4f* ClearColor;
        size_t ClearValueCount;
        float ClearDepth = 0.f;
        float ClearStencil = 0.f;
    };

    struct ViewportInfo
    {
        Tbx::Vector2f Transform;
        Tbx::Vector2f Size;
        float MinDepth;
        float MaxDepth;

        Tbx::Vector2i ScissorsOff;
        Tbx::Vector2ui Scissorsextent;
    };

    enum struct CommandBufferType
    {
        Primary,
        Secondary
    };

    struct CommandListCreateInfo
    {
        CommandPoolFamily CommandPoolFamily;
        CommandBufferType CommandBufferType;
    };

    struct MemoryBarrier
    {
        GpuAccessFlag SrcAccessMask;
        GpuAccessFlag DstAccessMask;
    };

    struct BufferMemoryBarrier
    {
        GpuAccessFlag SrcAccessMask;
        GpuAccessFlag DstAccessMask;
        RhiBuffer* Buffer;
        size_t Offset;
        size_t Size;
    };

    struct ImageMemoryBarrier
    {
        // TODO
        GpuAccessFlag SrcAccessMask;
        GpuAccessFlag DstAccessMask;

        ImageState CurrentState;
        ImageState NewState;

        RhiTexture* Texture;
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

        PC_CORE_API virtual void BeginRenderPass(const BeginRenderPassInfo& _beginRenderPassInfo);

        PC_CORE_API virtual void EndRenderPass() = 0;

        PC_CORE_API virtual void NextSubPass() = 0;

        PC_CORE_API virtual void BindDescriptorSet(const ShaderProgram* _shaderProgram,
                                                   const ShaderProgramDescriptorSets* _shaderProgramDescriptorSets,
                                                   size_t _firstSet, size_t _descriptorSetCount) = 0;

        PC_CORE_API virtual void BindProgram(const ShaderProgram* _shaderProgramm) = 0;

        PC_CORE_API virtual void PushConstant(const ShaderProgram* _shaderProgram, const std::string& _pushConstantKey,
                                              const void* _data, size_t _size) = 0;

        PC_CORE_API virtual void SetViewPort(const ViewportInfo& _viewPort) = 0;

        PC_CORE_API virtual void SetPrimitiveTopology(PrimitiveTopology _primitiveTopology) = 0;

        PC_CORE_API virtual void SetBlendEquation(uint32_t _firstAttachement, uint32_t _attachementCount) = 0;

        PC_CORE_API virtual void SetLineWidth(float _widht) = 0;

        PC_CORE_API virtual void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex,
                                      uint32_t _firstInstance) = 0;

        PC_CORE_API virtual void DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex,
                                             int32_t _vertexOffset, size_t _firstInstance) = 0;

        PC_CORE_API virtual void Dispatch(uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ) = 0;

        PC_CORE_API virtual void BindVertexBuffer(const RhiVertexBuffer& _vertexBuffer, uint32_t _firstBinding,
                                                  uint32_t _bindingCount) = 0;

        PC_CORE_API virtual void BindIndexBuffer(const RhiIndexBuffer& _indexBuffer, size_t _offset) = 0;

        PC_CORE_API virtual void CopyBuffer(const RhiBuffer& _src, const RhiBuffer& _dst, size_t _srcOffSet,
                                            size_t _dstoffset, size_t _sizeInBytes) = 0;

        PC_CORE_API virtual void Barrier(GpuPipelineStageFlagBits _srcStageMask, GpuPipelineStageFlagBits _dstStageMask,
                                         const MemoryBarrier* _memoryBarrier, size_t _memoryBarrierCount,
                                         const BufferMemoryBarrier* _bufferMemoryBarrier,
                                         size_t _bufferMemoryBarrierCount,
                                         const ImageMemoryBarrier* _imageMemoryBarrier,
                                         size_t _imageMemoryBarrierCount);

        PC_CORE_API void RecordFetchCommand(const std::function<void(CommandList*)>& _fectFunction);

        PC_CORE_API void ExecuteExternalCommand();

        PC_CORE_API virtual void Flush(FlushCommandMethod _flushCommandMethod,
                                       GpuPipelineStageFlagBits _waitGpuPipelineStageFlag) = 0;

        PC_CORE_API virtual void BeginDebugLabel(const char* _debugLabel, const std::array<float, 4>& _color) = 0;

        PC_CORE_API virtual void EndDebugLabel() = 0;

    protected:
        CommandPoolFamily m_CommandPoolFamily;

        CommandBufferType m_CommandBufferType;

        std::vector<std::function<void(CommandList*)>> m_FetchCommands;
    };


END_PCCORE
