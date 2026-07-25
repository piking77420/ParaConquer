#ifndef RENDERING_RENDERPASS_EQUIRECTANGULAR_TO_SKYBOX_HEADER
#define RENDERING_RENDERPASS_EQUIRECTANGULAR_TO_SKYBOX_HEADER

#include "RenderPass.hpp"

namespace PC_CORE::Rendering
{
    class Renderer;
    struct EnvironementData;
}

namespace PC_CORE::Rendering::Pass
{

    class PC_CORE_API BakeIbl : public PC_CORE::Rendering::RenderPass
    {
    public:

        BakeIbl();

        ~BakeIbl() override;

        IMP_DYNAMIC_REFLECT();

        const char* GetName() const
        {
            return "EquirectangularToSkybox";
        }

        std::array<float, 4> GetColor() const
        {
            return
            {
                0.1f,
                0.1f,
                0.7f,
                1.f
            };
        }

        void Build(const RendererPassBuildContext& _RendererPassBuildContext);

        void Execute(const RendererPassExecuteContext& _RendererPassExecuteContext);

    private:
        struct PassResource
        {
            PC_CORE::Rendering::PipelineCache::ModuleEntryList ModuleList;
            PC_CORE::Rendering::PipelineCacheID PipelineCacheID;
            RhiGraphicPipeline::Descriptor GraphicPipelineDescriptor;

            std::unique_ptr<RhiDescriptorSet> DescriptorSet;
            std::vector<std::array<std::unique_ptr<RhiFrameBuffer>, 6>> FrameBuffers;
        };
        
        struct BRDFPipiline{
            PC_CORE::Rendering::PipelineCache::ModuleEntryList ModuleList;
            PC_CORE::Rendering::PipelineCacheID PipelineCacheID;
            RhiGraphicPipeline::Descriptor GraphicPipelineDescriptor;
        }m_BRDFPipiline;

        std::array<Tbx::Matrix4x4f, 6> m_ViewMatricies;

        PassResource m_EquilateralToCubeMapResource;

        PassResource m_IrradianceConvolution;

        PassResource m_PrefilterMap;

        std::unique_ptr<RhiFrameBuffer> m_BRDFLUTFrameBuffer;
        
        void ComputeViewMatricies(const RendererPassBuildContext& _RendererPassBuildContext);

        [[nodiscard]] PassResource EquilateralToCubemapResource(const RendererPassExecuteContext& _RendererPassExecuteContext, const EnvironementData& _EnvironementData);

        [[nodiscard]] PassResource EnvironementResource(
            const RendererPassExecuteContext& _RendererPassExecuteContext, 
            const EnvironementData& _EnvironementData,
            const std::string& Name,
            RhiTexture& _Attachement);

        void ExecuteEquilateralToCubeMap(const RendererPassExecuteContext& _RendererPassExecuteContext);

        void ExecuteIrradiance(const RendererPassExecuteContext& _RendererPassExecuteContext);

        void ExecutePrefilter(const RendererPassExecuteContext& _RendererPassExecuteContext);

        void ExecuteBRDFLUT(const RendererPassExecuteContext& _RendererPassExecuteContext);

    };

    REFLECT(BakeIbl, PC_CORE::Rendering::RenderPass);
}


#endif // RENDERING_RENDERPASS_EQUIRECTANGULAR_TO_SKYBOX_HEADER