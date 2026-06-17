#include <Rendering/RenderPasses/BakeIbl.hpp>
#include <Rendering/RenderSystem.hpp>
#include <Rendering/Renderer.hpp>
#include <Rendering/RenderView.hpp>

namespace PC_CORE::Rendering::Pass
{
	BakeIbl::BakeIbl()
	{
		DYNAMIC_REFLECT_INIT
	}

	void BakeIbl::Build(const RendererPassBuildContext& _RendererPassBuildContext)
	{
		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Rendering);

		ComputeViewMatricies(_RendererPassBuildContext);
	}

	void BakeIbl::Execute(const RendererPassExecuteContext & _RendererPassExecuteContext)
	{
		auto& captureRenderPass = _RendererPassExecuteContext.RenderingWorldData.CaptureEnvironement;
		if (!captureRenderPass)
			return;

		if (!captureRenderPass->SkyBox)
			return;

		if (!captureRenderPass->isDirty)
			return;

		PERF_REGION_SCOPED;
		PERF_REGION_COLOR(PerfRegion::Rendering);

		m_EquilateralToCubeMapResource = EquilateralToCubemapResource(_RendererPassExecuteContext, *captureRenderPass);
		m_IrradianceConvolution = EnvironementResource(_RendererPassExecuteContext, *captureRenderPass, "Irradiance Convolution", *captureRenderPass->IrradianceMap);
		m_PrefilterMap = EnvironementResource(_RendererPassExecuteContext, *captureRenderPass, "Irradiance Convolution", *captureRenderPass->PrefilterMap);
		
		auto& BRDFLUT = captureRenderPass->BRDF;
		m_BRDFLUTFrameBuffer.reset(_RendererPassExecuteContext.RHI.CreateFrameBuffer());
		m_BRDFLUTFrameBuffer
			->SetWidth(BRDFLUT->GetWidth())
			.SetHeight(BRDFLUT->GetHeight())
			.SetAttachement(BRDFLUT)
			.SetRenderPass(_RendererPassExecuteContext.Renderer.BRDFLutPass.get())
			.SetName("BRDFLUT Framebuffer")
			.Build();

		// Set Base state
		_RendererPassExecuteContext.cmd.SetPrimitiveTopology(RhiShaderProgram::PrimitiveTopologyTriangleList);
		

		{
			PC_CORE::ViewportInfo ViewPortSkyBox(*captureRenderPass->SkyBox);
			_RendererPassExecuteContext.cmd.SetViewPort(ViewPortSkyBox);
			ExecuteEquilateralToCubeMap(_RendererPassExecuteContext);

			//why only need to transition the first level
			/*const ImageStateTransition ImageStateTransition
			{
				.Texture = captureRenderPass->SkyBox,
				.FirstMipLevel = 0,
				.MipLevelsCount = 1,
				.FirstLayer = 0,
				.LayerCount = captureRenderPass->SkyBox->GetLayer(),

				.updateState = true
			};
			_RendererPassExecuteContext.cmd.Barrier(RhiResourceState::PixelShaderResource, RhiResourceState::CopySrc, std::span(&ImageStateTransition, 1), {});*/
			//captureRenderPass->SkyBox->SetResourceState(RhiResourceState::PixelShaderResource);
			//captureRenderPass->SkyBox->GenerateMipMap(&_RendererPassExecuteContext.cmd, PC_CORE::Filter::Linear, RhiResourceState::PixelShaderResource);
		}
		
		{
			PC_CORE::ViewportInfo ViewPortIrradiance(*captureRenderPass->IrradianceMap);
			_RendererPassExecuteContext.cmd.SetViewPort(ViewPortIrradiance);
			ExecuteIrradiance(_RendererPassExecuteContext);
		}

		{
			ExecutePrefilter(_RendererPassExecuteContext);
		}

		{
			ExecuteBRDFLUT(_RendererPassExecuteContext);
		}
		
	}

	void BakeIbl::ComputeViewMatricies(const RendererPassBuildContext& _RendererPassBuildContext)
	{
		const Tbx::Matrix4x4f Proj = _RendererPassBuildContext.RHI.DepthCorrectionMatrixf() * Tbx::PerspectiveMatrixMinusOneToOne<float>(90.f * Tbx::dDeg2Rad, 1.0f, 0.1f, 10.f);
		for (size_t i = 0; i < m_ViewMatricies.size(); i++)
			m_ViewMatricies[i] = Proj * GetLookAtMatrixFromCubeMapIndicies(i, Tbx::Vector3f::Zero());
	}

	BakeIbl::PassResource BakeIbl::EquilateralToCubemapResource(const RendererPassExecuteContext& _RendererPassExecuteContext, const CaptureEnvironement& _CaptureEnvironement)
	{
		PassResource Resource;
		Resource.DescriptorSet = std::unique_ptr<RhiDescriptorSet>(_RendererPassExecuteContext.RHI.CreateDescriptorSet());
		Resource.DescriptorSet
			->BindTexture(RhiShaderStageBits::Pixel, 0, _CaptureEnvironement.Environement, _RendererPassExecuteContext.Renderer.linearClampToEdgeSampler.get())
			.SetName("EquirectangularToSkybox DescriptorSet")
			.Build();

		auto& Level1 = Resource.FrameBuffers.emplace_back();

		for (size_t i = 0; i < Level1.size(); i++)
		{
			Level1[i].reset(_RendererPassExecuteContext.RHI.CreateFrameBuffer());
			Level1[i]
				->SetWidth(_CaptureEnvironement.SkyBox->GetWidth())
				.SetHeight(_CaptureEnvironement.SkyBox->GetHeight())
				.SetAttachement(_CaptureEnvironement.SkyBox, RhiTexture::Type::Texture2D, static_cast<uint32_t>(i), 1, 0, 1)
				.SetRenderPass(_RendererPassExecuteContext.Renderer.colorHDRPass.get())
				.SetName("EquirectangularToSkybox Framebuffer" + std::to_string(i))
				.Build();
		}

		return Resource;
	}

	BakeIbl::PassResource BakeIbl::EnvironementResource(
		const RendererPassExecuteContext& _RendererPassExecuteContext,
		const CaptureEnvironement& _CaptureEnvironement,
		const std::string& Name,
		RhiTexture& _Attachement)
	{
		PassResource Resource;
		Resource.DescriptorSet = std::unique_ptr<RhiDescriptorSet>(_RendererPassExecuteContext.RHI.CreateDescriptorSet());
		Resource.DescriptorSet
			->BindTexture(RhiShaderStageBits::Pixel, 0, _CaptureEnvironement.SkyBox, _RendererPassExecuteContext.Renderer.linearClampToEdgeSampler.get())
			.SetName(Name + " DescriptorSet")
			.Build();

		for (size_t Level = 0; Level < _Attachement.GetLevel(); Level++)
		{
			auto& FrameBuffers = Resource.FrameBuffers.emplace_back();

			for (size_t i = 0; i < FrameBuffers.size(); i++)
			{
				const uint32_t MipWidth = _Attachement.GetWidth() * std::pow(0.5, Level);
				const uint32_t MipHeight = _Attachement.GetHeight() * std::pow(0.5, Level);

				FrameBuffers[i].reset(_RendererPassExecuteContext.RHI.CreateFrameBuffer());
				FrameBuffers[i]
					->SetWidth(MipWidth)
					.SetHeight(MipHeight)
					.SetAttachement(&_Attachement, RhiTexture::Type::Texture2D, static_cast<uint32_t>(i), 1, static_cast<uint32_t>(Level), 1)
					.SetRenderPass(_RendererPassExecuteContext.Renderer.colorHDRPass.get())
					.SetName(Name + "Framebuffer" + std::to_string(i))
					.Build();
			}
		}

		return Resource;
	}

	void BakeIbl::ExecuteEquilateralToCubeMap(const RendererPassExecuteContext& _RendererPassExecuteContext)
	{
		auto c = GetColor();
		auto& DescriptorSet = m_EquilateralToCubeMapResource.DescriptorSet;

		if (m_EquilateralToCubeMapResource.FrameBuffers.empty())
		{
			PC_LOGERROR("Failed To EquilateralTexture to cube map");
			return;
		}

		assert(m_EquilateralToCubeMapResource.FrameBuffers.size() == 1);

		for (size_t i = 0; i < m_EquilateralToCubeMapResource.FrameBuffers[0].size(); i++)
		{
			auto& FrameBuffer = m_EquilateralToCubeMapResource.FrameBuffers[0][i];
			const BeginRenderPassInfo beginRenderPassInfo =
			{
				.RenderPass = _RendererPassExecuteContext.Renderer.colorHDRPass.get(),
				.FrameBuffer = FrameBuffer.get(),
				.RenderOffSet = {0, 0},
				.Extent = {FrameBuffer->GetWidth(), FrameBuffer->GetHeight()},
				.ClearValueFlag = ClearValueFlagBits::ClearValueColor,
				.ClearColor = &c,
				.ClearValueCount = 1,
				.ClearDepth = 1.f
			};
			_RendererPassExecuteContext.cmd.BeginRenderPass(beginRenderPassInfo);
			_RendererPassExecuteContext.cmd.BindProgram(*_RendererPassExecuteContext.Renderer.EquilateralToSkyBox.get());
			_RendererPassExecuteContext.cmd.BindDescriptorSet(DescriptorSet.get(), 0);

			_RendererPassExecuteContext.cmd.PushConstant(RhiShaderStageBits::Vertex, &m_ViewMatricies[i], 0, sizeof(m_ViewMatricies[i]));

			_RendererPassExecuteContext.cmd.Draw(36, 1, 0, 0);
			_RendererPassExecuteContext.cmd.EndRenderPass();
		}

	}

	void BakeIbl::ExecuteIrradiance(const RendererPassExecuteContext& _RendererPassExecuteContext)
	{
		auto c = GetColor();
		auto& DescriptorSet = m_IrradianceConvolution.DescriptorSet;
		assert(m_IrradianceConvolution.FrameBuffers.size() == 1);


		for (size_t i = 0; i < m_IrradianceConvolution.FrameBuffers[0].size(); i++)
		{
			auto& FrameBuffer = m_IrradianceConvolution.FrameBuffers[0][i];
			const BeginRenderPassInfo beginRenderPassInfo =
			{
				.RenderPass = _RendererPassExecuteContext.Renderer.colorHDRPass.get(),
				.FrameBuffer = FrameBuffer.get(),
				.RenderOffSet = {0, 0},
				.Extent = {FrameBuffer->GetWidth(), FrameBuffer->GetHeight()},
				.ClearValueFlag = ClearValueFlagBits::ClearValueColor,
				.ClearColor = &c,
				.ClearValueCount = 1,
				.ClearDepth = 1.f
			};
			_RendererPassExecuteContext.cmd.BeginRenderPass(beginRenderPassInfo);
			_RendererPassExecuteContext.cmd.BindProgram(*_RendererPassExecuteContext.Renderer.IrradianceConvolution.get());
			_RendererPassExecuteContext.cmd.BindDescriptorSet(DescriptorSet.get(), 0);

			_RendererPassExecuteContext.cmd.PushConstant(RhiShaderStageBits::Vertex, &m_ViewMatricies[i], 0, sizeof(m_ViewMatricies[i]));

			_RendererPassExecuteContext.cmd.Draw(36, 1, 0, 0);
			_RendererPassExecuteContext.cmd.EndRenderPass();
		}
	}

	void BakeIbl::ExecutePrefilter(const RendererPassExecuteContext& _RendererPassExecuteContext)
	{
		auto c = GetColor();
		auto& DescriptorSet = m_PrefilterMap.DescriptorSet;

		for (size_t Level = 0; Level < m_PrefilterMap.FrameBuffers.size(); Level++)
		{
			auto& CubeMapLevel = m_PrefilterMap.FrameBuffers[Level];
			for (size_t Layer = 0; Layer < CubeMapLevel.size(); Layer++)
			{
				auto& CubeMapFace = CubeMapLevel[Layer];

				const BeginRenderPassInfo beginRenderPassInfo =
				{
					.RenderPass = _RendererPassExecuteContext.Renderer.colorHDRPass.get(),
					.FrameBuffer = CubeMapFace.get(),
					.RenderOffSet = {0, 0},
					.Extent = {CubeMapFace->GetWidth(), CubeMapFace->GetHeight()},
					.ClearValueFlag = ClearValueFlagBits::ClearValueColor,
					.ClearColor = &c,
					.ClearValueCount = 1,
					.ClearDepth = 1.f
				};

				GPU_ALIGN struct PrefilterData
				{
					Gpu::mat4 ViewProjectionCorr;
					float Roughness;
				}PrefilterData;
				
				std::memcpy(&PrefilterData.ViewProjectionCorr, m_ViewMatricies[Layer].data, sizeof(Gpu::mat4));
				PrefilterData.Roughness = static_cast<float>(Level) / static_cast<float>(m_PrefilterMap.FrameBuffers.size() - 1);

				PC_CORE::ViewportInfo ViewPortIrradiance(CubeMapFace->GetWidth(), CubeMapFace->GetHeight());

				_RendererPassExecuteContext.cmd.BeginRenderPass(beginRenderPassInfo);
				_RendererPassExecuteContext.cmd.SetViewPort(ViewPortIrradiance);
				_RendererPassExecuteContext.cmd.BindProgram(*_RendererPassExecuteContext.Renderer.PrefilterEnvironement.get());
				_RendererPassExecuteContext.cmd.BindDescriptorSet(DescriptorSet.get(), 0);
				_RendererPassExecuteContext.cmd.PushConstant(RhiShaderStageBits::Vertex | RhiShaderStageBits::Pixel, &PrefilterData, 0, sizeof(PrefilterData));
				_RendererPassExecuteContext.cmd.Draw(36, 1, 0, 0);
				_RendererPassExecuteContext.cmd.EndRenderPass();
			}
		}

	}

	void BakeIbl::ExecuteBRDFLUT(const RendererPassExecuteContext& _RendererPassExecuteContext)
	{
		auto& FrameBuffer = m_BRDFLUTFrameBuffer;
		auto c = GetColor();
		const BeginRenderPassInfo beginRenderPassInfo =
		{
			.RenderPass = _RendererPassExecuteContext.Renderer.BRDFLutPass.get(),
			.FrameBuffer = FrameBuffer.get(),
			.RenderOffSet = {0, 0},
			.Extent = {FrameBuffer->GetWidth(), FrameBuffer->GetHeight()},
			.ClearValueFlag = ClearValueFlagBits::ClearValueColor,
			.ClearColor = &c,
			.ClearValueCount = 1,
			.ClearDepth = 1.f
		};
		_RendererPassExecuteContext.cmd.SetViewPort(PC_CORE::ViewportInfo(FrameBuffer->GetWidth(), FrameBuffer->GetHeight()));
		_RendererPassExecuteContext.cmd.BeginRenderPass(beginRenderPassInfo);
		_RendererPassExecuteContext.cmd.BindProgram(*_RendererPassExecuteContext.Renderer.BRDFLutPipeline.get());
		_RendererPassExecuteContext.cmd.Draw(3, 1, 0, 0);
		_RendererPassExecuteContext.cmd.EndRenderPass();
	}

} // namespace PC_CORE::Rendering::Pass