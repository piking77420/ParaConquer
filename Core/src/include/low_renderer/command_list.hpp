#pragma once

#include <memory>

#include "core_header.hpp"
#include "frame_buffer.hpp"
#include "rhi_render_pass.hpp"
#include "math/toolbox_typedef.hpp"

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
    std::shared_ptr<PC_CORE::RhiRenderPass> frameBuffer;
    Tbx::Vector2i renderOffSet;
    Tbx::Vector2ui extent;

    Tbx::Vector4f clearColor;
        
};

class CommandList
{
public:
    CommandList(CommandPoolFamily _commandPoolFamily);
    
    CommandList() = default;

    virtual ~CommandList() = default;

    virtual void BeginRecordCommands();

    virtual void BeginRenderPass(const PC_CORE::BeginRenderPassInfo& _BeginRenderPassInfo) = 0;

    virtual void EndRenderPass() = 0;

protected:
    CommandPoolFamily m_CommandPoolFamily;
};

END_PCCORE