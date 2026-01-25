#pragma once

#include "RenderPass.hpp"



namespace PC_CORE::Rendering
{
    class Renderer;
    struct StaticMeshComponentData;
}

namespace PC_CORE::Rendering::Pass
{

    class PC_CORE_API FowardPass : public PC_CORE::Rendering::RenderPass
    {
    public:

        FowardPass();

        ~FowardPass() override = default;


        IMP_DYNAMIC_REFLECT();

        const char* GetName() const
        {
            return "FowardPass";
        }

        std::array<float, 4> GetColor() const
        {
            return
            {
                0.f,
                0.f,
                1.f,
                1.f
            };
        }

        void Build(const RendererPassBuildContext& _RendererPassBuildContext);

        void Execute(const RendererPassExecuteContext& _RendererPassExecuteContext) const;

    private:
        std::unique_ptr<RhiFrameBuffer> m_FrameBuffer;

        std::unique_ptr<RhiDescriptorSet> m_DescriptorSet;

        RhiTexture* m_LightingImage = nullptr;

        mutable std::vector<std::pair<double, uint32_t>> m_TransparentSubMeshDistanceV;

    };

    REFLECT(FowardPass, PC_CORE::Rendering::RenderPass);
}
