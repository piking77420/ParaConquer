#pragma once

#include "Rendering/RenderingTypedef.h"

namespace PC_CORE
{
class World;
}

namespace PC_CORE::Rendering
{
class RenderGraph;

class PC_CORE_API View
{
public:
    DEFAULT_COPY_MOVE_OPERATIONS(View)

    explicit View(PC_CORE::Rhi& _Rhi, Tbx::Vector2i _viewPortSize);

    virtual ~View() = default;

    virtual void DeclarePass(Rendering::RenderGraph* RenderGraph) = 0;

    virtual void UpdateView() = 0;

    Tbx::Vector2i renderSize{};

    std::unique_ptr<RhiBuffer> SceneBufferUbo;
protected:
    PC_CORE::Rhi& m_Rhi;

};

REFLECT(View);
}
