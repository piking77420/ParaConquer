#pragma once

#include "rendering_typedef.h"
#include "camera.hpp"
#include "gbuffers.hpp"
#include "buffer/uniform_buffer.hpp"

BEGIN_PCCORE

class Renderer;

class PC_CORE_API View
{
public:

	void SetRenderingContextFlag(size_t _flag);

	void SetCamera(PC_CORE::Camera* _camera);

	void Resize(Tbx::Vector2i _viewPortSize);

	void Update();

	const RenderingContext& GetRenderingContext() const
	{
		return m_RenderingContext;
	}

	const CameraGpu& GetCameraGpu() const
	{
		return m_CameraGpu;
	}

	const PostProcessGpu& GetPostProcessGpu() const
	{
		return m_PostProcessGpu;
	}

	Texture2D& GetFinalImage()
	{
		return m_FinalImage;
	}

	DEFAULT_COPY_MOVE_OPERATIONS(View)

	View(Renderer* _renderer, Tbx::Vector2i _viewPortSize);

	View() = default;

	~View() = default;

private:
	struct ForwardTexture
	{
		Texture2D color;
		Texture2D depth;
	};

	struct DescriptorSets
	{
		ShaderProgramDescriptorSets* geometryPass = nullptr;

		ShaderProgramDescriptorSets* defferedPassGbuffers = nullptr;
		ShaderProgramDescriptorSets* defferedPassCameraLight = nullptr;

		ShaderProgramDescriptorSets* forwardDescriptor = nullptr;

		ShaderProgramDescriptorSets* toneMap = nullptr;
		ShaderProgramDescriptorSets* finalViewPort = nullptr;
	};

	struct FrameBuffers
	{
		std::shared_ptr<FrameBuffer> gbufferFrameBuffer;
		std::shared_ptr<FrameBuffer> forwardFrameBuffer;
		std::shared_ptr<FrameBuffer> finalImageFrameBuffer;
	};


	Renderer* m_Renderer;

	PC_CORE::Camera* m_Camera;

	RenderingContext m_RenderingContext;

	Tbx::Vector2i m_CurrentSize;

	CameraGpu m_CameraGpu;

	PostProcessGpu m_PostProcessGpu;

	Gbuffers m_Gbuffers;

	ForwardTexture m_ForwardTexture;

	Texture2D m_FinalImage;

	Texture2D m_ResolvedImages;

	FrameBuffers m_FrameBuffers;

	DescriptorSets m_DescriptorSets;

	void UpdateRenderingContext();

	void CreateImages();

	void CreateFrameBuffers();

	void CreateDescritproSets();

};
REFLECT(View);

END_PCCORE
