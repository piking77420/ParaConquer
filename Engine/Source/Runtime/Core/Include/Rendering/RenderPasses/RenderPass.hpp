#pragma once

#include "Reflection/DynamicReflectable.hpp"
#include "LowRenderer/RhiRenderPass.hpp"

namespace PC_CORE::Rendering
{
class RenderGraphContext;
class RenderGraph;

class PC_CORE_API RenderPass : public DynamicReflectable
{
public:
    RenderPass();

    ~RenderPass() override = default;

    IMP_DYNAMIC_REFLECT()

    virtual void Build(RenderGraph& RenderGraph) = 0;

    virtual void Update(const RenderGraphContext& _RenderGraphContext) = 0;

    virtual void Execute(const RenderGraphContext& _RenderGraphContext) = 0;

    bool IsDisable = false;

protected:
};

REFLECT(RenderPass, PC_CORE::DynamicReflectable);


}