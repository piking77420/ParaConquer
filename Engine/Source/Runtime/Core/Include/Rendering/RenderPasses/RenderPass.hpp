#pragma once
#include <memory>

#include "Guid.hpp"
#include "Reflection/DynamicReflectable.hpp"
#include "LowRenderer/RhiRenderPass.hpp"
#include "Primitive/AABB.hpp"

namespace PC_CORE 
{
    class CommandList;
    class Rhi;
    class RhiDescriptorSet;
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
        RenderGraph& RenderGraph;
        const RenderView& View;
        const Renderer& Renderer;
    };

    struct RendererPassExecuteContext
    {
        CommandList& cmd;
        Rhi& RHI;
        RenderGraph& RenderGraph;
        const RenderView& View;
        const Renderer& Renderer;
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

    size_t PickLodCount(const std::vector<double>& LodThreshold, const MotionCore::Aabb<double>& AABBW, const Tbx::Vector3d& CameraPos, double FovRad) const;

protected:
};

template <typename Derived>
concept RenderPassT = std::is_base_of_v<RenderPass, std::remove_cvref_t<Derived>> && std::is_base_of_v<DynamicReflectable, std::remove_cvref_t<Derived>>;

REFLECT(RenderPass, PC_CORE::DynamicReflectable);

}

