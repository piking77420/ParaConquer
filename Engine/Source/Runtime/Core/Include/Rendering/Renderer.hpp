#pragma once
#include <Rendering/DrawList.hpp>
#include "RenderingTypedef.h"
#include "RenderGraph.hpp"
#include <Rendering/Buffer/IndexBuffer.hpp>
#include <Rendering/Buffer/VertexBuffer.hpp>
#include <DebugHelper/DebugDrawContext.hpp>
#include <Frustum.hpp>

namespace PC_CORE::Rendering
{

class PipelineCache;
class RenderView;

class Renderer
{
public:
    struct DebugPrimitive
    {
        VertexBuffer VertexBuffer;
        IndexBuffer IndexBuffer;
        std::unique_ptr<RhiBuffer> InstanceBuffer;
    };

    PC_CORE_API explicit Renderer(Rhi& _Rhi, PC_CORE::Window& Window);

    PC_CORE_API ~Renderer() = default;

    PC_CORE_API void RenderFrame();

    PC_CORE_API void Build(const RenderView& _View, const std::function<void(RenderGraph&)>& InitRenderGraphFunction);

    PC_CORE_API void Excute(RenderView& _view, PipelineCache& PipelineCache, const RenderingWorldData& RenderingWorldData);

    const RenderGraph& GetRenderGraph() const
    {
        return m_RenderGraph;
    }
    PC_CORE_API void SetPipelineCache(PC_CORE::Rendering::PipelineCache* _PipelineCache);

    static size_t PickLodCount(const std::vector<double>& LodThreshold, double BoundingSphereRadius, double AABBDistanceToCam, double FovRad);  

    std::function<void(CommandList&)> OnSwapchainPass;

    std::function<void(CommandList&)> OnRender;

    DrawList OpaqueList;

    DrawList TransparentList;

    DrawList DebugDrawList;

    DrawList Skybox;

    PipelineCache* PipelinesCache{nullptr};

    std::unique_ptr<RhiSampler> linearClampToEdgeSampler;

    std::unique_ptr<RhiRenderPass> forwardPass;

    std::unique_ptr<RhiRenderPass> colorLinearPass;

    std::unique_ptr<RhiRenderPass> colorHDRPass;

    std::unique_ptr<RhiRenderPass> BRDFLutPass;

    std::unique_ptr<RhiRenderPass> LinearClearColorClearStoreDepth;

    std::unique_ptr<RhiRenderPass> LoadHdrColorLoadStoreDepth;

    std::unique_ptr<RhiBuffer> InstanceBuffer;

    std::unique_ptr<RhiDescriptorSet> SkyBoxDescriptorSet;

    std::unique_ptr<RhiDescriptorSet> EnvironementDescriptorSet;

    std::array<DebugPrimitive, static_cast<size_t>(DebugDrawContext::PrimitiveType::Count)> m_DebugPrimitiveBuffer;
private:
    Rhi& m_Rhi;

    PC_CORE::Window& m_Window;

    RenderGraph m_RenderGraph; // TODO SET IT OUTSIT ThE CLASS

    std::unique_ptr<CommandList> m_CommandList;

    static constexpr double FIXED_POINT_NUMBER = 1000.0f;

    static constexpr size_t MAX_DEBUG_INSTANCE = 4096;

    std::vector<Tbx::Matrix4x4f> DebugInstanceBuffer;

    std::vector<Gpu::RenderInstance> m_InstanceBufferCpu;

    void InitRhiRenderPasses();

    void BuildDrawLists(RenderView& _view, const RenderingWorldData& RenderingWorldData);

    void UploadRenderInstanceID();

    void UpdateEnvironement(RenderView& _view, const RenderingWorldData& RenderingWorldData);

    void PrepareInstanceBuffer(const RenderingWorldData& RenderingWorldData);

    void FillListStaticMesh(RenderView& _view, const RenderingWorldData& RenderingWorldData);

    void FillListDebugDraw(const RenderView& _view, const RenderingWorldData& RenderingWorldData);

    void FillSkyBox(const RenderView& _view, const RenderingWorldData& RenderingWorldData);

    void SortList();

    void InitDebugResource();    
};

} // PC_CORE::Rendering
