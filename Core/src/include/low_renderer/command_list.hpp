#pragma once

#include <memory>

#include "core_header.hpp"
#include "frame_buffer.hpp"
#include "rhi_render_pass.hpp"
#include "math/toolbox_typedef.hpp"
#include "resources/shader_program.h"

BEGIN_PCCORE
    enum class CommandPoolFamily
{
    Graphics,
    Compute,
    Count
};

struct BeginRenderPassInfo
{
    std::shared_ptr<PC_CORE::RhiRenderPass> renderPass;
    std::shared_ptr<PC_CORE::FrameBuffer> frameBuffer;
    Tbx::Vector2i renderOffSet;
    Tbx::Vector2ui extent;

    Tbx::Vector4f clearColor;
        
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

    PC_CORE_API virtual void BindProgram(const ShaderProgram* _shaderProgramm) = 0;

    PC_CORE_API virtual void SetViewPort(const ViewportInfo& _viewPort) = 0;

    PC_CORE_API virtual void Draw(uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance) = 0;

protected:
    CommandPoolFamily m_CommandPoolFamily;
};


END_PCCORE
