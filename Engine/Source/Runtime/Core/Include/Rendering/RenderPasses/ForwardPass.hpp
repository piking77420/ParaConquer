#pragma once

#include "DrawPass.hpp"

namespace PC_CORE::Rendering::Pass
{

    class PC_CORE_API FowardPass : public DrawPass
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
                0.5f,
                0.5f,
                0.5f,
                1.f
            };
        }

        void Build(const RendererPassBuildContext& _RendererPassBuildContext);

        void Execute(const RendererPassExecuteContext& _RendererPassExecuteContext) const;

    private:
        std::unique_ptr<RhiFrameBuffer> m_FrameBuffer;

        std::unique_ptr<RhiDescriptorSet> m_DescriptorSet;

        RhiTexture* m_LightingImage = nullptr;
    };

    REFLECT(FowardPass, DrawPass);
}
