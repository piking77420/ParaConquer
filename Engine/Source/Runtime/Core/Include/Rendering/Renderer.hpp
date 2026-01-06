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

    PC_CORE_API void Build(const RenderView& _view);

    PC_CORE_API void Excute(RenderView& _view, const RenderingWorldData& RenderingWorldData);

    const RenderGraph& GetRenderGraph() const
    {
        return m_RenderGraph;
    }

    std::unique_ptr<RhiShaderProgram> drawTextureQuad;

    std::unique_ptr<RhiShaderProgram> fowardShader;

    std::unique_ptr<RhiSampler> linearClampToEdgeSampler;

    std::unique_ptr<RhiRenderPass> forwardPass;

    std::unique_ptr<RhiRenderPass> toneMapPass;

private:
    Rhi& m_Rhi;

    RenderGraph m_RenderGraph; // TODO SET IT OUTSIT ThE CLASS

    std::unique_ptr<CommandList> m_CommandList;

    void InitRhiRenderPasses(const RenderView& _View);

    void InitShaders(const RenderView& _View);
    
};

} // PC_CORE::Rendering
