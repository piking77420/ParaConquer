#include <LowRenderer/RhiGraphicPipeline.hpp>
#include <LowRenderer/RhiRenderPass.hpp>

namespace PC_CORE 
{
	RhiGraphicPipeline::RhiGraphicPipeline(Rhi& _Rhi) 
		: RhiPipeline(_Rhi)
	{

	}

	RhiGraphicPipeline::~RhiGraphicPipeline() = default;

	RhiGraphicPipeline& RhiGraphicPipeline::FromDescriptor(const Descriptor& _Descriptor)
	{
		m_Descriptor = _Descriptor;
		return *this;
	}

	uint32_t PC_CORE::RhiGraphicPipeline::Descriptor::Hash() const
    {
        uint32_t Seed = 0;
        HashCombine(Seed, static_cast<uint32_t>(PolygonMode));
        HashCombine(Seed, static_cast<uint32_t>(CullMode));
        HashCombine(Seed, static_cast<uint32_t>(Sample));
        HashCombine(Seed, static_cast<uint32_t>(FrontFace));
        if (DephStencilState)
            HashCombine(Seed, static_cast<uint32_t>(DephStencilState->Hash()));
        if (BlendState)
            HashCombine(Seed, static_cast<uint32_t>(BlendState->Hash()));
        if (RenderPass)
            HashCombine(Seed, static_cast<uint32_t>(RenderPass->Hash()));
        HashCombine(Seed, static_cast<uint32_t>(SubPassIndex));

        for (const auto& it : VertexInputBindingDescritions)
            HashCombine(Seed, it.Hash());

        for (const auto& it : VertexInputBindingDescritions)
            HashCombine(Seed, it.Hash());

        return Seed;
    }
	
} // namespace PC_CORE

