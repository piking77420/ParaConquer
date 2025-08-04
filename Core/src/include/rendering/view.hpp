#pragma once

#include "rendering_typedef.h"
#include "camera.hpp"
#include "gbuffers.hpp"
#include "buffer/uniform_buffer.hpp"

BEGIN_PCCORE

class Renderer;

struct ForwardTexture
{
	Texture2D color;
	Texture2D depth;
};


class PC_CORE_API View
{
public:
	PostProcessGpu postProcessGpu;

	CameraGpu cameraGpu;

	RenderingContext renderingContext;

	Gbuffers gbuffers;

	ForwardTexture forwardTexture;

	Texture2D finalImage;

	Texture2D resolvedImages;

	void SetRenderingContextFlag(size_t _flag);

	void SetCamera(PC_CORE::Camera* _camera);

	void Resize(Tbx::Vector2i _viewPortSize);

	void Update();

	
	DEFAULT_COPY_MOVE_OPERATIONS(View)

	View(Renderer* _renderer, Tbx::Vector2i _viewPortSize);

	View() = default;

	~View() = default;

private:
	
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

	Tbx::Vector2i m_CurrentSize;

	FrameBuffers m_FrameBuffers;

	DescriptorSets m_DescriptorSets;

	void UpdateRenderingContext();

	void CreateImages();

	void CreateFrameBuffers();

	void CreateDescritproSets();

};
REFLECT(View);

END_PCCORE
