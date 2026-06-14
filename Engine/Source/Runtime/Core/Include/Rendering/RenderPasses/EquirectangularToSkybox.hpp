#ifndef RENDERING_RENDERPASS_EQUIRECTANGULAR_TO_SKYBOX_HEADER
#define RENDERING_RENDERPASS_EQUIRECTANGULAR_TO_SKYBOX_HEADER

#include "RenderPass.hpp"

namespace PC_CORE::Rendering
{
    class Renderer;
    struct CaptureEnvironement;
}

namespace PC_CORE::Rendering::Pass
{

    class PC_CORE_API EquirectangularToSkybox : public PC_CORE::Rendering::RenderPass
    {
    public:

        EquirectangularToSkybox();

        ~EquirectangularToSkybox() override = default;

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
            std::unique_ptr<RhiDescriptorSet> DescriptorSet;
            std::vector<std::array<std::unique_ptr<RhiFrameBuffer>, 6>> FrameBuffers;
        };
        
        std::array<Tbx::Matrix4x4f, 6> m_ViewMatricies;

        PassResource m_EquilateralToCubeMapResource;

        PassResource m_IrradianceConvolution;

        PassResource m_PrefilterMap;

        std::unique_ptr<RhiFrameBuffer> m_BRDFLUTFrameBuffer = nullptr;
        
        void ComputeViewMatricies(const RendererPassBuildContext& _RendererPassBuildContext);

        [[nodiscard]] PassResource EquilateralToCubemapResource(const RendererPassExecuteContext& _RendererPassExecuteContext, const CaptureEnvironement& _CaptureEnvironement);

        [[nodiscard]] PassResource EnvironementResource(
            const RendererPassExecuteContext& _RendererPassExecuteContext, 
            const CaptureEnvironement& _CaptureEnvironement,
            const std::string& Name,
            RhiTexture& _Attachement);

        void ExecuteEquilateralToCubeMap(const RendererPassExecuteContext& _RendererPassExecuteContext);

        void ExecuteIrradiance(const RendererPassExecuteContext& _RendererPassExecuteContext);

        void ExecutePrefilter(const RendererPassExecuteContext& _RendererPassExecuteContext);

        void ExecuteBRDFLUT(const RendererPassExecuteContext& _RendererPassExecuteContext);

    };

    REFLECT(EquirectangularToSkybox, PC_CORE::Rendering::RenderPass);
}


#endif // RENDERING_RENDERPASS_EQUIRECTANGULAR_TO_SKYBOX_HEADER