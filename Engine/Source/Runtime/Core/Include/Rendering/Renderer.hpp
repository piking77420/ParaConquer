#pragma once
#include "RenderingTypedef.h"
#include "RenderGraph.hpp"

namespace PC_CORE::Rendering
{

class RenderView;

class Renderer
{
public:
    PC_CORE_API explicit Renderer(Rhi& _Rhi);

    PC_CORE_API ~Renderer() = default;

    DEFAULT_COPY_MOVE_OPERATIONS(Renderer)

    PC_CORE_API void Update(const RenderView& _view);

    PC_CORE_API void Excute(const RenderView& _view);

private:
    Rhi& m_Rhi;

    RenderGraph m_RenderGraph;
    
};

} // PC_CORE::Rendering
