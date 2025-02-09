#pragma once

#include <memory>
#include <functional>

#include "core_header.hpp"
#include "frame_buffer.hpp"
#include "index_buffer.hpp"
#include "rhi_render_pass.hpp"
#include "math/toolbox_typedef.hpp"
#include "resources/shader_program.h"
#include "vertex_buffer.hpp"

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
    Tbx::Vector4f clearColor;
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

struct CommandListCreateInfo
{
    CommandPoolFamily _commandPoolFamily;
};

class CommandList
{
public:

    DEFAULT_COPY_MOVE_OPERATIONS(CommandList)

    PC_CORE_API CommandList(const CommandListCreateInfo& _commandListCreateInfo);
    
    PC_CORE_API CommandList() = default;

    PC_CORE_API virtual ~CommandList() = default;

    PC_CORE_API virtual void Reset() = 0;

    PC_CORE_API virtual void BeginRecordCommands() = 0;

    PC_CORE_API virtual void EndRecordCommands() = 0;

    PC_CORE_API virtual void BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo) = 0;

    PC_CORE_API virtual void EndRenderPass() = 0;
    
    PC_CORE_API virtual void BindDescriptorSet(const ShaderProgram* _shaderProgram, const ShaderProgramDescriptorSets* _shaderProgramDescriptorSets,
        size_t _firstSet, size_t _descriptorSetCount) = 0;

    PC_CORE_API virtual void BindProgram(const ShaderProgram* _shaderProgramm) = 0;

    PC_CORE_API virtual void PushConstant(const std::string& _pushConstantKey, const PC_CORE::ShaderProgram* _shaderProgram,
        void* _data, size_t _size) = 0;

    PC_CORE_API virtual void SetViewPort(const ViewportInfo& _viewPort) = 0;

    PC_CORE_API virtual void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance) = 0;

    PC_CORE_API virtual void DrawIndexed(size_t _indexCount, size_t _instanceCount, size_t _firstIndex, int32_t _vertexOffset, size_t _firstInstance) = 0;

    PC_CORE_API virtual void BindVertexBuffer(const VertexBuffer& _vertexBuffer, uint32_t _firstBinding, uint32_t _bindingCount) = 0;

    PC_CORE_API virtual void BindIndexBuffer(const IndexBuffer& _indexBuffer, size_t _offset) = 0;

    PC_CORE_API void RecordFetchCommand(std::function<void(CommandList*)> _fectFunction);

    PC_CORE_API void ExucuteFetchCommand();

protected:
    CommandPoolFamily m_CommandPoolFamily;

    std::vector<std::function<void(CommandList*)>> m_FecthCommands;
};


END_PCCORE
