#pragma once

#include "Reflection/DynamicReflectable.hpp"
#include "LowRenderer/RhiRenderPass.hpp"

namespace PC_CORE 
{
    class CommandList;
}

namespace PC_CORE::Rendering
{
    class RenderView;
    class RenderingWorldData;


    using RenderPassGetNameFunc = const char* (*)(void*);

    using RenderPassBuildFunc = void (*)(void*, const RenderView&);

    using RenderPassExecuteFunc = void (*)(void*, CommandList*, const RenderView&, const RenderingWorldData&);


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

