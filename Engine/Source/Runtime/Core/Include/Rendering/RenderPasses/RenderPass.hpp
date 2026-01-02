#pragma once

#include "Guid.hpp"
#include "Reflection/DynamicReflectable.hpp"
#include "LowRenderer/RhiRenderPass.hpp"

namespace PC_CORE 
{
    class CommandList;
    class Rhi;
    class ShaderProgramDescriptorSets;
    class RhiFrameBuffer;
}

namespace PC_CORE::Rendering
{
    class RenderView;
    class RenderingWorldData;
    class Renderer;
    class RenderGraph;

    struct RendererPassBuildContext
    {
        CommandList& cmd;
        Rhi& RHI;
        const RenderView& View;
        const Renderer& Renderer;
        RenderGraph& RenderGraph;
    };

    struct RendererPassExecuteContext
    {
        CommandList& cmd;
        Rhi& RHI;
        const RenderView& View;
        const Renderer& Renderer;
        const RenderGraph& RenderGraph;
        const RenderingWorldData& RenderingWorldData;
    };

    using RenderPassGetNameFunc = const char* (*)(const void*);

    using RenderPassGetColorFunc = std::array<float, 4> (*)(const void*);

    using RenderPassBuildFunc = void (*)(void*, const RendererPassBuildContext&);

    using RenderPassExecuteFunc = void (*)(const void*, const RendererPassExecuteContext&);

class PC_CORE_API RenderPass : public DynamicReflectable
{
public:
    RenderPass();

    ~RenderPass() override = default;

    IMP_DYNAMIC_REFLECT()

    bool IsDisable = false;

protected:
};

template <typename Derived>
concept RenderPassT = std::is_base_of_v<RenderPass, std::remove_cvref_t<Derived>>;

REFLECT(RenderPass, PC_CORE::DynamicReflectable);

}

