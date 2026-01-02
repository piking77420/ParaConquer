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

    const RenderGraph& GetRenderGraph() const
    {
        return m_RenderGraph;
    }

    std::unique_ptr<RhiShaderProgram> drawTextureQuad;

    std::unique_ptr<RhiShaderProgram> fowardShader;

    std::unique_ptr<RhiSampler> linearClampToEdgeSampler;

private:
    Rhi& m_Rhi;

    RenderGraph m_RenderGraph;

    Pass::FowardPass m_FowardPass;

    Pass::ToneMapPass m_ToneMapPass;

    std::unique_ptr<CommandList> m_CommandList;

    void InitShaders(const RenderView& _View);
    
};

} // PC_CORE::Rendering
