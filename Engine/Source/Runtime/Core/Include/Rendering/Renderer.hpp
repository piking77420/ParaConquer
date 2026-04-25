#pragma once
#include <Rendering/DrawList.hpp>
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

    PC_CORE_API void Build(const RenderView& _View, const std::function<void(RenderGraph&)>& InitRenderGraphFunction);

    PC_CORE_API void Excute(RenderView& _view, const RenderingWorldData& RenderingWorldData);

    const RenderGraph& GetRenderGraph() const
    {
        return m_RenderGraph;
    }

    DrawList OpaqueList;

    DrawList TransparentList;

    std::unique_ptr<RhiShaderProgram> drawTextureQuad;

    std::unique_ptr<RhiShaderProgram> toneMapAces;

    std::unique_ptr<RhiShaderProgram> opaqueFowardShader;

    std::unique_ptr<RhiShaderProgram> transparentForwardShader;

    std::unique_ptr<RhiShaderProgram> opaqueFowardShaderMeshlet;

    std::unique_ptr<RhiShaderProgram> transparentForwardShaderMeshlet;

    std::unique_ptr<RhiShaderProgram> DrawMeshletColor;

    std::unique_ptr<RhiShaderProgram> DrawTriangle;

    std::unique_ptr<RhiShaderProgram> DrawMeshTriangleMeshlet;

    std::unique_ptr<RhiSampler> linearClampToEdgeSampler;

    std::unique_ptr<RhiRenderPass> forwardPass;

    std::unique_ptr<RhiRenderPass> colorLinearPass;

    std::unique_ptr<RhiRenderPass> colorLinearPassDepth;

    std::unique_ptr<RhiBuffer> InstanceBuffer;

    std::vector<Gpu::RenderInstance> InstanceBufferCpu;

private:
    Rhi& m_Rhi;

    RenderGraph m_RenderGraph; // TODO SET IT OUTSIT ThE CLASS

    std::unique_ptr<CommandList> m_CommandList;

    static constexpr double FIXED_POINT_NUMBER = 1000.0f;

    void InitRhiRenderPasses();

    void InitShaders();

    void BuildDrawLists(RenderView& _view, const RenderingWorldData& RenderingWorldData);

    void UploadRenderInstanceID();

    void FillListStaticMesh(RenderView& _view, const RenderingWorldData& RenderingWorldData);

    void SortList();

    size_t PickLodCount(const std::vector<double>& LodThreshold, double BoundingSphereRadius, double AABBDistanceToCam, double FovRad) const;
    
};

} // PC_CORE::Rendering
