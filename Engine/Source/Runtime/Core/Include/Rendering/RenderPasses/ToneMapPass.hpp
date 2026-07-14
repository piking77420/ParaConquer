#pragma once

#include "RenderPass.hpp"

namespace PC_CORE::Rendering
{
    class Renderer;
}

namespace PC_CORE::Rendering::Pass
{

    class PC_CORE_API ToneMapPass : public PC_CORE::Rendering::RenderPass
    {
    public:

        ToneMapPass();

        ~ToneMapPass() override = default;


        IMP_DYNAMIC_REFLECT();

        const char* GetName() const
        {
            return "ToneMapPass";
        }

        std::array<float, 4> GetColor() const
        {
            return
            {
                0.3f,
                0.5f,
                0.2f,
                1.f
            };
        }

        void Build(const RendererPassBuildContext& _RendererPassBuildContext);

        void Execute(const RendererPassExecuteContext& _RendererPassExecuteContext);

    private:
        RhiTexture* m_LightingImageRef{ nullptr };
        RhiTexture* m_OutPutImageRef{ nullptr };

        std::unique_ptr<PC_CORE::RhiDescriptorSet> m_DesciptorSetToneMap;

        std::unique_ptr<RhiFrameBuffer> m_FrameBuffer;

        PipelineCache::ModuleEntryList m_ModuleEntryList;

        PipelineCacheID m_PipelineCacheID;

        void ToneMapPassExecute(const RendererPassExecuteContext& _RendererPassExecuteContext);
    };

    REFLECT(ToneMapPass, PC_CORE::Rendering::RenderPass);
}
