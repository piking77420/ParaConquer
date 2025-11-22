#pragma once

#include <Array>
#include <String>
#include <Memory>
#include <Functional>
#include <span>

#include "CoreHeader.hpp"
#include "Math/ToolboxTypedef.hpp"

#include "RhiFrameBuffer.hpp"
#include "RhiRenderPass.hpp"
#include "Rendering/Buffer/VertexBuffer.hpp"

BEGIN_PCCORE
    class RhiFrameBuffer;
    class RhiFence;
    class RhiShaderProgram;


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
        std::shared_ptr<RhiFrameBuffer> FrameBuffer;
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
        Tbx::Vector2f Transform{ 0,0 };
        Tbx::Vector2f Size{ 0,0 };
        float MinDepth = 0.0f;
        float MaxDepth = 1.0;

        Tbx::Vector2i ScissorsOff{ 0,0 };
        Tbx::Vector2ui ScissorsExtent{ 0,0 };

        explicit ViewportInfo(RhiTexture& texture)
        {
            const uint32_t widht = texture.GetWidth();
            const uint32_t height = texture.GetHeight();

            Size = Tbx::Vector2f(static_cast<float>(widht), static_cast<float>(height));
            ScissorsExtent = Tbx::Vector2ui(widht, height);
        }

        explicit ViewportInfo(Tbx::Vector2ui size)
        {
            Size = Tbx::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y));
            ScissorsExtent = size;
        }

        ViewportInfo() = default;

        ~ViewportInfo() = default;
        
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



    enum struct FlushCommandMethod
    {
        Sync, // will flush command at once at the end of tick
        Async, // send directly to gpu // TODO
    };

   
    class CommandList : public RhiObject
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(CommandList)

        PC_CORE_API explicit CommandList(Rhi& _Rhi, const std::string& _name, const CommandListCreateInfo& _commandListCreateInfo);

        PC_CORE_API explicit CommandList(Rhi& _Rhi, std::string&& _name, const CommandListCreateInfo& _commandListCreateInfo);

        PC_CORE_API virtual ~CommandList() = default;

        PC_CORE_API virtual void Reset() = 0;

        PC_CORE_API virtual void MergeCommands(CommandList* _secondaries, size_t _count) = 0;

        PC_CORE_API virtual void BeginRecordCommands() = 0;

        PC_CORE_API virtual void EndRecordCommands() = 0;

        PC_CORE_API virtual void BeginRenderPass(const BeginRenderPassInfo& _beginRenderPassInfo);

        PC_CORE_API virtual void EndRenderPass() = 0;

        PC_CORE_API virtual void NextSubPass() = 0;

        PC_CORE_API virtual void BindDescriptorSet(const RhiShaderProgram& _RhiShaderProgram,
                                                   const ShaderProgramDescriptorSets* _shaderProgramDescriptorSets,
                                                   size_t _firstSet, size_t _descriptorSetCount) = 0;

        PC_CORE_API virtual void BindProgram(const RhiShaderProgram& _RhiShaderProgram) = 0;

        PC_CORE_API virtual void PushConstant(const RhiShaderProgram& _RhiShaderProgram, const std::string& _pushConstantKey,
                                              const void* _data, size_t _size) = 0;

        PC_CORE_API virtual void SetViewPort(const ViewportInfo& _viewPort) = 0;

        PC_CORE_API virtual void SetPrimitiveTopology(PC_CORE::RhiShaderProgram::PrimitiveTopology _primitiveTopology) = 0;

        PC_CORE_API virtual void SetBlendEquation(uint32_t _firstAttachement, uint32_t _attachementCount) = 0;

        PC_CORE_API virtual void SetLineWidth(float _widht) = 0;

        PC_CORE_API virtual void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex,
                                      uint32_t _firstInstance) = 0;

        PC_CORE_API virtual void DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex,
                                             int32_t _vertexOffset, size_t _firstInstance) = 0;

        PC_CORE_API virtual void Dispatch(uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ) = 0;

        PC_CORE_API virtual void BindVertexBuffer(const RhiBuffer& _vertexBuffer, uint32_t _firstBinding,
                                                  uint32_t _bindingCount) = 0;

        PC_CORE_API virtual void BindIndexBuffer(const RhiBuffer& _indexBuffer, RhiBuffer::IndexFormat _format, size_t _offset) = 0;

        PC_CORE_API virtual void CopyBuffer(const RhiBuffer& _src, const RhiBuffer& _dst, size_t _srcOffSet,
                                            size_t _dstoffset, size_t _sizeInBytes) = 0;

        PC_CORE_API virtual void Barrier(GpuPipelineStage _srcStageMask, GpuPipelineStage _DstStageMask,                 
                                          const std::span<ImageStateTransition>& _ImageStateTransition,
                                          const std::span<BufferStateTransition>& _BufferStateTransition) = 0;

        PC_CORE_API void RecordFetchCommand(const std::function<void(CommandList*)>& _fectFunction);

        PC_CORE_API void ExecuteExternalCommand();

        PC_CORE_API virtual void Flush(PC_CORE::FlushCommandMethod _flushCommandMethod,
                                       PC_CORE::GpuPipelineStage _waitGpuPipelineStageFlag) = 0;

        PC_CORE_API virtual void Flush(RhiFence& _fence) = 0;

        PC_CORE_API virtual void BeginDebugLabel(const char* _debugLabel, const std::array<float, 4>& _color) = 0;

        PC_CORE_API virtual void EndDebugLabel() = 0;

    protected:
        CommandPoolFamily m_CommandPoolFamily;

        CommandBufferType m_CommandBufferType;

        std::vector<std::function<void(CommandList*)>> m_FetchCommands;
    };


END_PCCORE
