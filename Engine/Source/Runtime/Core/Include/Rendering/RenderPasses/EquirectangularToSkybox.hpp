#ifndef RENDERING_RENDERPASS_EQUIRECTANGULAR_TO_SKYBOX_HEADER
#define RENDERING_RENDERPASS_EQUIRECTANGULAR_TO_SKYBOX_HEADER

#include "RenderPass.hpp"

namespace PC_CORE::Rendering
{
    class Renderer;
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
        std::unique_ptr<RhiDescriptorSet> m_DescriptorSet;
        std::array<std::unique_ptr<RhiFrameBuffer>, 6> m_FrameBuffer;
    };

    REFLECT(EquirectangularToSkybox, PC_CORE::Rendering::RenderPass);
}


#endif // RENDERING_RENDERPASS_EQUIRECTANGULAR_TO_SKYBOX_HEADER