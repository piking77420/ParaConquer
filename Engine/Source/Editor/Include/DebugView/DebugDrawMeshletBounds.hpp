#ifndef PC_EDITOR_DEBUG_DRAW_MESHLET_BOUNDS
#define PC_EDITOR_DEBUG_DRAW_MESHLET_BOUNDS

#include <memory>
#include <Rendering/RenderPasses/DrawPass.hpp>
#include <LowRenderer/RhiFrameBuffer.hpp>
#include <LowRenderer/RhiDescriptorSet.hpp>
#include <LowRenderer/RhiShaderProgram.hpp>


namespace PC_EDITOR::DebugView
{
    namespace Pass = PC_CORE::Rendering::Pass;

    class DebugDrawMeshletBounds final : public Pass::DrawPass
    {
    public:
        DebugDrawMeshletBounds();

        ~DebugDrawMeshletBounds() override;

        IMP_DYNAMIC_REFLECT()

        const char* GetName() const
        {
            return "DebugDrawMeshletBounds";
        }

        std::array<float, 4> GetColor() const
        {
            return
            {
                0.8f,
                0.1f,
                0.12f,
                1.f
            };
        }

        void Build(const PC_CORE::Rendering::RendererPassBuildContext& _RendererPassBuildContext);

        void Execute(const PC_CORE::Rendering::RendererPassExecuteContext& _RendererPassExecuteContext);

    private:
        std::unique_ptr<PC_CORE::RhiFrameBuffer> m_FrameBuffer;

        std::unique_ptr<PC_CORE::RhiDescriptorSet> m_DescriptorSet;
    };

    REFLECT(DebugDrawMeshletBounds, PC_CORE::Rendering::Pass::DrawPass);

} // namespace PC_EDITOR::DebugView

#endif // PC_EDITOR_DEBUG_DRAW_MESHLET_BOUNDS
