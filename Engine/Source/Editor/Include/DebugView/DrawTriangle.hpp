#ifndef PC_EDITOR_DEBUG_VIEW_TRIANGLE
#define PC_EDITOR_DEBUG_VIEW_TRIANGLE

#include <memory>
#include <Rendering/RenderPasses/DrawPass.hpp>
#include <LowRenderer/RhiFrameBuffer.hpp>
#include <LowRenderer/RhiDescriptorSet.hpp>

namespace PC_EDITOR::DebugView
{
	class Triangle final: public PC_CORE::Rendering::Pass::DrawPass
	{
	public:
		Triangle();
		~Triangle() override;

		IMP_DYNAMIC_REFLECT()

        const char* GetName() const
        {
            return "DebugView Triangle";
        }

        std::array<float, 4> GetColor() const
        {
            return
            {
                0.5f,
                0.8f,
                0.1f,
                1.f
            };
        }

        void Build(const PC_CORE::Rendering::RendererPassBuildContext& _RendererPassBuildContext);

        void Execute(const PC_CORE::Rendering::RendererPassExecuteContext& _RendererPassExecuteContext) const;

	private:
        std::unique_ptr<PC_CORE::RhiFrameBuffer> m_FrameBuffer;

        std::unique_ptr<PC_CORE::RhiDescriptorSet> m_DescriptorSet;

        std::unique_ptr<PC_CORE::RhiDescriptorSet> m_MeshShaderDescriptorSet;
	};

    REFLECT(Triangle, PC_CORE::Rendering::Pass::DrawPass);

} // namespace PC_EDITOR::DebugView

#endif // PC_EDITOR_DEBUG_VIEW_TRIANGLE
