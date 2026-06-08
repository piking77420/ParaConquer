#include <Rendering/RenderPasses/EquirectangularToSkybox.hpp>
#include <Rendering/RenderSystem.hpp>
#include <Rendering/Renderer.hpp>
#include <Rendering/RenderView.hpp>

namespace PC_CORE::Rendering::Pass
{
	EquirectangularToSkybox::EquirectangularToSkybox()
	{
		DYNAMIC_REFLECT_INIT
	}

	void EquirectangularToSkybox::Build(const RendererPassBuildContext& _RendererPassBuildContext)
	{

	}

	void EquirectangularToSkybox::Execute(const RendererPassExecuteContext & _RendererPassExecuteContext)
	{
		auto& captureRenderPass = _RendererPassExecuteContext.RenderingWorldData.CaptureEnvironement;
		if (!captureRenderPass)
			return;

		if (!captureRenderPass->SkyBox)
			return;

		if (!captureRenderPass->isDirty)
			return;

		m_DescriptorSet.reset(_RendererPassExecuteContext.RHI.CreateDescriptorSet());
		m_DescriptorSet
			->BindTexture(RhiShaderStageBits::Pixel, 0, captureRenderPass->Environement, _RendererPassExecuteContext.Renderer.linearClampToEdgeSampler.get())
			.SetName("EquirectangularToSkybox DescriptorSet")
			.Build();

		for (size_t i = 0; i < m_FrameBuffer.size(); i++)
		{
			m_FrameBuffer[i].reset(_RendererPassExecuteContext.RHI.CreateFrameBuffer());
			m_FrameBuffer[i]
				->SetWidth(captureRenderPass->SkyBox->GetWidth())
				.SetHeight(captureRenderPass->SkyBox->GetHeight())
				.SetAttachement(captureRenderPass->SkyBox, RhiTexture::Type::Texture2D, static_cast<uint32_t>(i), 1, 0, 1)
				.SetRenderPass(_RendererPassExecuteContext.Renderer.colorHDRPass.get())
				.SetName("EquirectangularToSkybox Framebuffer" + std::to_string(i))
				.Build();
		}

		_RendererPassExecuteContext.cmd.SetPrimitiveTopology(RhiShaderProgram::PrimitiveTopologyTriangleList);

		PC_CORE::ViewportInfo ViewPort(*captureRenderPass->SkyBox);

		_RendererPassExecuteContext.cmd.SetViewPort(ViewPort);
		const Tbx::Matrix4x4f Proj = Tbx::PerspectiveMatrixFlipYAxis<float>(90.f * Tbx::dDeg2Rad, 1.0f, 0.1f, 10.f);
		for (size_t i = 0; i < m_FrameBuffer.size(); i++)
		{
			auto c = GetColor();
			const BeginRenderPassInfo beginRenderPassInfo =
			{
				.RenderPass = _RendererPassExecuteContext.Renderer.colorHDRPass.get(),
				.FrameBuffer = m_FrameBuffer[i].get(),
				.RenderOffSet = {0, 0},
				.Extent = {m_FrameBuffer[i]->GetWidth(), m_FrameBuffer[i]->GetHeight()},
				.ClearValueFlag = ClearValueFlagBits::ClearValueColor,
				.ClearColor = &c,
				.ClearValueCount = 1,
				.ClearDepth = 1.f
			};
			_RendererPassExecuteContext.cmd.BeginRenderPass(beginRenderPassInfo);
			_RendererPassExecuteContext.cmd.BindProgram(*_RendererPassExecuteContext.Renderer.EquilateralToSkyBox.get());
			_RendererPassExecuteContext.cmd.BindDescriptorSet(m_DescriptorSet.get(), 0);

			const Tbx::Matrix4x4f ViewProjection = Proj * GetLookAtMatrixFromCubeMapIndicies(i, Tbx::Vector3f::Zero());
			_RendererPassExecuteContext.cmd.PushConstant(RhiShaderStageBits::Pixel, &ViewProjection, 0, sizeof(ViewProjection));

			_RendererPassExecuteContext.cmd.Draw(36, 1, 0, 0);
			_RendererPassExecuteContext.cmd.EndRenderPass();
		}

	}



} // namespace PC_CORE::Rendering::Pass