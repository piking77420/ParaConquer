#pragma once

#include "CoreHeader.hpp"
#include "LowRenderer/RhiRenderPass.hpp"



namespace PC_CORE::Rendering
{
class RenderGraphContext;

class RenderPass : public DynamicReflectable
{
public:
    PC_CORE_API RenderPass();

    PC_CORE_API ~RenderPass() override = default;

    IMP_DYNAMIC_REFLECT()

    const RhiRenderPass& operator*() const
    {
        return *m_RhiRenderPass;
    }

    RhiRenderPass& operator*()
    {
        return *m_RhiRenderPass;
    }

    RhiRenderPass* operator->()
    {
        return m_RhiRenderPass.get();
    }

    const RhiRenderPass* operator->() const
    {
        return m_RhiRenderPass.get();
    }

    RhiRenderPass* Get()
    {
        return m_RhiRenderPass.get();
    }

    const RhiRenderPass* Get() const
    {
        return m_RhiRenderPass.get();
    }

    virtual void Build(RenderGraph& RenderGraph) = 0;

    virtual void Update(const RenderGraphContext& _RenderGraphContext) = 0;

    virtual void Execute(const RenderGraphContext& _RenderGraphContext) = 0;

    bool IsDisable = false;

protected:
    std::unique_ptr<RhiRenderPass> m_RhiRenderPass;
};

REFLECT(RenderPass, PC_CORE::DynamicReflectable);


}