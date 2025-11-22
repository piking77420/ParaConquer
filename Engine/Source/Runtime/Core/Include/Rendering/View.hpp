#pragma once

#include "RenderingTypedef.h"
#include "Camera.hpp"
#include "Gbuffers.hpp"
#include "Buffer/UniformBuffer.hpp"

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
        PostProcessGpu PostProcessGpu;

        CameraGpu CameraGpu;

        RenderingContext RenderingContext;

        Gbuffers Gbuffers;

        ForwardTexture ForwardTexture;

        Texture2D FinalImage;

        Texture2D ResolvedImages;

        void SetRenderingContextFlag(size_t _flag);

        void SetCamera(Camera* _camera);

        void Resize(Tbx::Vector2i _viewPortSize);

        void Update();


        DEFAULT_COPY_MOVE_OPERATIONS(View)

        View(Renderer* _renderer, Tbx::Vector2i _viewPortSize);

        View() = default;

        ~View() = default;

    private:
        struct DescriptorSets
        {
            ShaderProgramDescriptorSets* GeometryPass = nullptr;

            ShaderProgramDescriptorSets* DefferedPassGbuffers = nullptr;
            ShaderProgramDescriptorSets* DefferedPassCameraLight = nullptr;

            ShaderProgramDescriptorSets* ForwardDescriptor = nullptr;

            ShaderProgramDescriptorSets* ToneMap = nullptr;
            ShaderProgramDescriptorSets* FinalViewPort = nullptr;
        };

        struct FrameBuffers
        {
            std::shared_ptr<FrameBuffer> GbufferFrameBuffer;
            std::shared_ptr<FrameBuffer> ForwardFrameBuffer;
            std::shared_ptr<FrameBuffer> FinalImageFrameBuffer;
        };

        Renderer* m_Renderer;

        Camera* m_Camera;

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
