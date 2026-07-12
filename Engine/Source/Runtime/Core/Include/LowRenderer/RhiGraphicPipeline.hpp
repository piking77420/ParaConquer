#ifndef PC_RHI_RHIGRAPHIC_PIPELINE
#define PC_RHI_RHIGRAPHIC_PIPELINE

#include <LowRenderer/RhiPipeline.hpp>

namespace PC_CORE
{ 
	class RhiGraphicPipeline : public RhiPipeline
	{
	public:
        struct Descriptor;
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


        struct Descriptor
        {
            PolygonMode PolygonMode{ PolygonMode::Fill };
            CullModeFlag CullMode{ 0u };
            uint32_t Sample{ 1u };
            FrontFace FrontFace{ FrontFace::CounterClockwise };
            std::optional<DephStencilInfo> DephStencilInfo;
            std::optional<BlendState> BlendState;
            PrimitiveTopology PrimitiveTopology{ PrimitiveTopology::PrimitiveTopologyTriangleList };

            RhiRenderPass* RenderPass{ nullptr };
            uint32_t SubPassIndex{ 0u };

            std::vector<VertexInputBindingDescrition> VertexInputBindingDescritions;
            std::vector<VertexAttributeDescription> VertexAttributeDescriptions;

             PC_CORE_API Descriptor& SetPolygonMode(RhiGraphicPipeline::PolygonMode _PolygonMode)
             {
                 PolygonMode = _PolygonMode;
                 return *this;
             }

             PC_CORE_API Descriptor& SetCullMode(CullModeFlag _CullMode)
             {
                 CullMode = _CullMode;
                 return *this;
             }

             PC_CORE_API Descriptor& SetSamples(uint32_t _Sample)
             {
                 Sample = _Sample;
                 return *this;
             }

             PC_CORE_API Descriptor& SetFrontFace(RhiGraphicPipeline::FrontFace _FrontFace)
             {
                 FrontFace = _FrontFace;
                 return *this;
             }

             PC_CORE_API Descriptor& SetDepthTest(bool _DepthTest)
             {
                 if (!DephStencilInfo.has_value())
                 {
                     DephStencilInfo.emplace();
                 }


                 DephStencilInfo->enableDepthTest = _DepthTest;
                 return *this;
             }

             PC_CORE_API Descriptor& SetDepthWrite(bool _DepthWrite)
             {
                 if (!DephStencilInfo.has_value())
                 {
                     DephStencilInfo.emplace();
                 }
                 DephStencilInfo->enableDepthWrite = _DepthWrite;
                 return *this;
             }

             PC_CORE_API Descriptor& SetDepthCompareOp(CompareOp _CompareOp)
             {
                 if (!DephStencilInfo.has_value())
                 {
                     DephStencilInfo.emplace();
                 }

                 DephStencilInfo->depthCompareOp = _CompareOp;
                 return *this;
             }

             PC_CORE_API Descriptor& SetBlendState(RhiGraphicPipeline::BlendState _BlendInfo)
             {
                 BlendState.emplace(_BlendInfo);
                 return *this;
             }

             PC_CORE_API Descriptor& SetRenderPass(RhiRenderPass& _RhiRenderPass)
             {
                 RenderPass = &_RhiRenderPass;
                 return *this;
             }

             PC_CORE_API Descriptor& SetSubPassIndex(uint32_t _SubPassIndex)
             {
                 SubPassIndex = _SubPassIndex;
                 return *this;
             }

             PC_CORE_API Descriptor& SetVertexInputBindingDescritions(const std::vector<VertexInputBindingDescrition>& _VertexInputBindingDescritions)
             {
                 VertexInputBindingDescritions = _VertexInputBindingDescritions;
                 return *this;
             }

             PC_CORE_API Descriptor& SetVertexAttributeDescriptions(const std::vector<VertexAttributeDescription>& _VertexAttributeDescriptions)
             {
                VertexAttributeDescriptions = _VertexAttributeDescriptions;
                return *this;
             }

             uint32_t Hash() const;
        };

        PC_CORE_API RhiGraphicPipeline& SetPolygonMode(PolygonMode _PolygonMode)
        {
            m_Descriptor.SetPolygonMode(_PolygonMode);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetCullMode(CullModeFlag _CullMode)
        {
            m_Descriptor.SetCullMode(_CullMode);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetSamples(uint32_t _Sample)
        {
            m_Descriptor.SetSamples(_Sample);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetFrontFace(FrontFace _FrontFace)
        {
            m_Descriptor.SetFrontFace(_FrontFace);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetDepthTest(bool _DepthTest)
        {
            m_Descriptor.SetDepthTest(_DepthTest);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetDepthWrite(bool _DepthWrite)
        {
            m_Descriptor.SetDepthWrite(_DepthWrite);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetDepthCompareOp(CompareOp _CompareOp)
        {
            m_Descriptor.SetDepthCompareOp(_CompareOp);

            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetBlendState(BlendState _BlendInfo)
        {
            m_Descriptor.SetBlendState(_BlendInfo);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetRenderPass(RhiRenderPass& _RhiRenderPass)
        {
            m_Descriptor.SetRenderPass(_RhiRenderPass);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetSubPassIndex(uint32_t _SubPassIndex)
        {
            m_Descriptor.SetSubPassIndex(_SubPassIndex);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetVertexInputBindingDescritions(const std::vector<VertexInputBindingDescrition>& _VertexInputBindingDescritions)
        {
            m_Descriptor.SetVertexInputBindingDescritions(_VertexInputBindingDescritions);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& SetVertexAttributeDescriptions(const std::vector<VertexAttributeDescription>& _VertexAttributeDescriptions)
        {
            m_Descriptor.SetVertexAttributeDescriptions(_VertexAttributeDescriptions);
            return *this;
        }

        PC_CORE_API RhiGraphicPipeline& FromDescriptor(const Descriptor& _Descriptor);

    protected:
        Descriptor m_Descriptor;
	};

} // namespace PC_CORE


#endif // !PC_RHI_RHIGRAPHIC_PIPELINE
