#ifndef PC_RHI_RHIGRAPHIC_PIPELINE
#define PC_RHI_RHIGRAPHIC_PIPELINE

#include <LowRenderer/RhiPipeline.hpp>

namespace PC_CORE
{ 
	class RhiGraphicPipeline : public RhiPipeline
	{
	public:
		RhiGraphicPipeline(Rhi& _Rhi);
		virtual ~RhiGraphicPipeline();

        enum class PolygonMode
        {
            Fill,
            Line,
            Point,
            FillRectangleNV
        };

        enum PrimitiveTopology
        {
            PrimitiveTopologyPointList = 0,
            PrimitiveTopologyLineList = 1,
            PrimitiveTopologyLineStrip = 2,
            PrimitiveTopologyTriangleList = 3,
            PrimitiveTopologyTriangleStrip = 4,
            PrimitiveTopologyTriangle_FAN = 5,
            PrimitiveTopologyLineListWithAdjacency = 6,
            PrimitiveTopologyLineStripWithAdjacency = 7,
            PrimitiveTopologyTriangleListWithAdjacency = 8,
            PrimitiveTopologyTriangleStripWithAdjacency = 9,
            PrimitiveTopologyPathList = 10,

            PrimitiveTopologyCount
        };

        enum CullModeFlagBits
        {
            CullNone = 0,
            CullFront = 1 << 1,
            CullBack = 1 << 2,
            CullFrontAndBack = 1 << 3,
        };

        using CullModeFlag = uint32_t;

        enum class FrontFace
        {
            CounterClockwise,
            Clockwise
        };

        struct BlendState
        {
            BlendFactor ColorSrcFactor = BlendFactor::SrcAlpha;
            BlendFactor ColorDstFactor = BlendFactor::OneMinusSrcAlpha;
            BlendOp ColorOp = BlendOp::Add;

            BlendFactor AlphaSrcFactor = BlendFactor::One;
            BlendFactor AlphaDstFactor = BlendFactor::OneMinusSrcAlpha;
            BlendOp AlphaOp = BlendOp::Add;

            ColorComponent BlendMask = ColorComponent::ColorComponentRGBA;

            BlendState& SetColorSrcFactor(BlendFactor _BlendFactor)
            {
                ColorSrcFactor = _BlendFactor;
                return *this;
            }

            BlendState& SetColorDstFactor(BlendFactor _BlendFactor)
            {
                ColorDstFactor = _BlendFactor;
                return *this;
            }

            BlendState& SetColorBlendOp(BlendOp _BlendOp)
            {
                ColorOp = _BlendOp;
                return *this;
            }

            BlendState& SetAlphaSrcFactor(BlendFactor _BlendFactor)
            {
                AlphaSrcFactor = _BlendFactor;
                return *this;
            }

            BlendState& SetAlphaDstFactor(BlendFactor _BlendFactor)
            {
                AlphaDstFactor = _BlendFactor;
                return *this;
            }

            BlendState& SetAlphaBlendOp(BlendOp _BlendOp)
            {
                ColorOp = _BlendOp;
                return *this;
            }

            BlendState& SetBlendMask(PC_CORE::ColorComponent _BlendMask)
            {
                BlendMask = _BlendMask;
                return *this;
            }

            uint32_t Hash() const
            {
                uint32_t Seed = 0;
                HashCombine(Seed, static_cast<uint32_t>(ColorSrcFactor));
                HashCombine(Seed, static_cast<uint32_t>(ColorDstFactor));
                HashCombine(Seed, static_cast<uint32_t>(ColorOp));

                HashCombine(Seed, static_cast<uint32_t>(AlphaSrcFactor));
                HashCombine(Seed, static_cast<uint32_t>(AlphaDstFactor));
                HashCombine(Seed, static_cast<uint32_t>(AlphaOp));

                HashCombine(Seed, static_cast<uint32_t>(BlendMask));
                return Seed;
            }

        };

        struct DephStencilInfo
        {
            bool enableDepthTest = false;
            bool enableDepthWrite = false;
            CompareOp depthCompareOp = CompareOp::Less;

            uint32_t Hash() const
            {
                uint32_t Seed = 0;
                HashCombine(Seed, static_cast<uint32_t>(enableDepthTest));
                HashCombine(Seed, static_cast<uint32_t>(enableDepthWrite));
                HashCombine(Seed, static_cast<uint32_t>(depthCompareOp));
                return Seed;
            }
        };

        PC_CORE_API RhiGraphicPipeline& SetPolygonMode(PolygonMode _PolygonMode)
        {
            m_PolygonMode = _PolygonMode;
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetCullMode(CullModeFlag _CullMode)
        {
            m_CullMode = _CullMode;
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetSamples(uint32_t _Sample)
        {
            m_Sample = _Sample;
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetFrontFace(FrontFace _FrontFace)
        {
            m_FrontFace = _FrontFace;
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetDepthTest(bool _DepthTest)
        {
            if (!m_DephStencilInfo.has_value())
            {
                m_DephStencilInfo.emplace();
            }


            m_DephStencilInfo->enableDepthTest = _DepthTest;
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetDepthWrite(bool _DepthWrite)
        {
            if (!m_DephStencilInfo.has_value())
            {
                m_DephStencilInfo.emplace();
            }
            m_DephStencilInfo->enableDepthWrite = _DepthWrite;
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetDepthCompareOp(CompareOp _CompareOp)
        {
            if (!m_DephStencilInfo.has_value())
            {
                m_DephStencilInfo.emplace();
            }

            m_DephStencilInfo->depthCompareOp = _CompareOp;
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetBlendState(BlendState _BlendInfo)
        {
            m_BlendState.emplace(_BlendInfo);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetRenderPass(RhiRenderPass& _RhiRenderPass)
        {
            m_RenderPass = &_RhiRenderPass;
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetSubPassIndex(uint32_t _SubPassIndex)
        {
            m_SubPassIndex = _SubPassIndex;
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetVertexInputBindingDescritions(const std::vector<VertexInputBindingDescrition>& _VertexInputBindingDescritions)
        {
            m_VertexInputBindingDescritions = _VertexInputBindingDescritions;
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetVertexAttributeDescriptions(const std::vector<VertexAttributeDescription>& _VertexAttributeDescriptions)
        {
            m_VertexAttributeDescriptions = _VertexAttributeDescriptions;
            return *this;
        }

    protected:
        PolygonMode m_PolygonMode{ PolygonMode::Fill };
        CullModeFlag m_CullMode{ 0u };
        uint32_t m_Sample{ 1u };
        FrontFace m_FrontFace{ FrontFace::CounterClockwise };
        std::optional<DephStencilInfo> m_DephStencilInfo;
        std::optional<BlendState> m_BlendState;
        PrimitiveTopology m_PrimitiveTopology{ PrimitiveTopology::PrimitiveTopologyTriangleList };

        RhiRenderPass* m_RenderPass{ nullptr };
        uint32_t m_SubPassIndex{ 0u };

        std::vector<VertexInputBindingDescrition> m_VertexInputBindingDescritions;
        std::vector<VertexAttributeDescription> m_VertexAttributeDescriptions;

	};

} // namespace PC_CORE


#endif // !PC_RHI_RHIGRAPHIC_PIPELINE
