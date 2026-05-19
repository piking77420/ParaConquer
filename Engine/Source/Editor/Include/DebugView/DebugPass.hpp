#ifndef PC_EDITOR_DEBUG_PASS
#define PC_EDITOR_DEBUG_PASS

#include <memory>
#include <Rendering/RenderPasses/DrawPass.hpp>
#include <LowRenderer/RhiFrameBuffer.hpp>
#include <LowRenderer/RhiDescriptorSet.hpp>
#include <LowRenderer/RhiShaderProgram.hpp>


namespace PC_EDITOR::DebugView
{
    namespace Pass = PC_CORE::Rendering::Pass;

	class DebugPass final : public Pass::DrawPass
	{
	public:
        DebugPass();

		DebugPass(const std::string& _Name,
            const std::array<float, 4>& _GpuColor,
            std::unique_ptr<PC_CORE::RhiShaderProgram>*& _ShaderProgramTriangle,
            std::unique_ptr<PC_CORE::RhiShaderProgram>*& _ShaderProgramMeshlet);

		~DebugPass() override;

		IMP_DYNAMIC_REFLECT()

        const char* GetName() const
        {
            return m_Name.c_str();
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

        std::string m_Name;

        std::array<float, 4> m_GpuDebugerColor;

        std::unique_ptr<PC_CORE::RhiShaderProgram>* m_ShaderProgramTriangle{ nullptr };

        std::unique_ptr<PC_CORE::RhiShaderProgram>* m_ShaderProgramMeshlet{ nullptr };
	};

    REFLECT(DebugPass, PC_CORE::Rendering::Pass::DrawPass);

} // namespace PC_EDITOR::DebugView

#endif // PC_EDITOR_DEBUG_PASS
