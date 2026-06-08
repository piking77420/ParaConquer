#ifndef PC_EDITOR_DEBUG_SHAPE_DRAW
#define PC_EDITOR_DEBUG_SHAPE_DRAW

#include <Rendering/RenderPasses/RenderPass.hpp>

namespace PC_EDITOR::DebugView
{

	class DebugShapeDraw : public PC_CORE::Rendering::RenderPass
	{
	public:
		DebugShapeDraw();
		virtual ~DebugShapeDraw();

		IMP_DYNAMIC_REFLECT()

        const char* GetName() const
        {
            return "DebugShapeDraw";
        }

        std::array<float, 4> GetColor() const
        {
            return
            {
                0.2f,
                0.8f,
                0.8f,
                1.f
            };
        }

        void Build(const PC_CORE::Rendering::RendererPassBuildContext& _RendererPassBuildContext);

        void Execute(const PC_CORE::Rendering::RendererPassExecuteContext& _RendererPassExecuteContext);

	private:
        std::unique_ptr<PC_CORE::RhiFrameBuffer> m_FrameBuffer;

        std::unordered_map<std::string_view, std::unique_ptr<PC_CORE::RhiDescriptorSet>> m_DescriptorSets;

        std::unique_ptr<PC_CORE::RhiDescriptorSet> m_DescriptorSet;

	};

	REFLECT(DebugShapeDraw, PC_CORE::Rendering::RenderPass);

} // namespace PC_EDITOR::DebugView

#endif // PC_EDITOR_DEBUG_SHAPE_DRAW