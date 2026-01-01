#pragma once
#include "RenderingTypedef.h"
#include "RenderGraph.hpp"
#include "Rendering/RenderPasses/ForwardPass.hpp"
#include "Rendering/RenderPasses/ToneMapPass.hpp"

namespace PC_CORE::Rendering
{

class RenderView;

class Renderer
{
public:
    PC_CORE_API explicit Renderer(Rhi& _Rhi);

    PC_CORE_API ~Renderer() = default;

    DEFAULT_COPY_MOVE_OPERATIONS(Renderer)

    PC_CORE_API void Build(const RenderView& _view);

    PC_CORE_API void Excute(const RenderView& _view);

    std::unique_ptr<RhiShaderProgram> drawTextureQuad;

private:
    Rhi& m_Rhi;

    RenderGraph m_RenderGraph;

    Pass::FowardPass m_FowardPass;

    Pass::ToneMapPass m_ToneMapPass;

    std::unique_ptr<CommandList> m_CommandList;

    void InitShaders();
    
};

} // PC_CORE::Rendering
