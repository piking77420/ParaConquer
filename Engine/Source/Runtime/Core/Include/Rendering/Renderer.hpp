#pragma once
#include "RenderingTypedef.h"


namespace PC_CORE
{
    class Window;
}

namespace PC_CORE::Rendering
{

   

class RenderGraph;
class View;


class Renderer
{
public:
    PC_CORE_API Renderer() = default;

    PC_CORE_API ~Renderer();

    PC_CORE_API void Init(Rhi& _Rhi);

    PC_CORE_API void Update(const View& _view, const RenderGraph& RenderGraph);

    PC_CORE_API void Excute(const View& _view, const RenderGraph& RenderGraph);




private:
    Rhi* m_Rhi{nullptr};
    
};

} // PC_CORE::Rendering
