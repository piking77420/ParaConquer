#pragma once

#include "RenderingTypedef.h"
#include "Camera.hpp"
#include "Gbuffers.hpp"
#include "Buffer/UniformBuffer.hpp"

BEGIN_PCCORE
    class Renderer;

    struct ForwardTexture
    {
        std::unique_ptr<RhiTexture> color;
        std::unique_ptr<RhiTexture> depth;
    };


    class PC_CORE_API View
    {
    public:
        PostProcessGpu PostProcessGpu;

        CameraGpu CameraGpu;

        RenderingContext RenderingContext;

        Gbuffers Gbuffers;

        ForwardTexture ForwardTexture;

        std::unique_ptr<RhiTexture> FinalImage;

        std::unique_ptr<RhiTexture> ResolvedImages;

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
            std::unique_ptr<ShaderProgramDescriptorSets> GeometryPass = nullptr;

            std::unique_ptr<ShaderProgramDescriptorSets> DefferedPassGbuffers = nullptr;
            std::unique_ptr<ShaderProgramDescriptorSets> DefferedPassCameraLight = nullptr;

            std::unique_ptr<ShaderProgramDescriptorSets> ForwardDescriptor = nullptr;

            std::unique_ptr<ShaderProgramDescriptorSets> ToneMap = nullptr;
            std::unique_ptr<ShaderProgramDescriptorSets> FinalViewPort = nullptr;
        };

        struct FrameBuffers
        {
            std::shared_ptr<RhiFrameBuffer> GbufferFrameBuffer;
            std::shared_ptr<RhiFrameBuffer> ForwardFrameBuffer;
            std::shared_ptr<RhiFrameBuffer> FinalImageFrameBuffer;
        };

        Renderer* m_Renderer = nullptr;

        Camera* m_Camera = nullptr;

        Tbx::Vector2i m_CurrentSize{};

        FrameBuffers m_FrameBuffers{};

        DescriptorSets m_DescriptorSets{};

        void UpdateRenderingContext();

        void CreateImages();

        void CreateFrameBuffers();

        void CreateDescritproSets();
    };

    REFLECT(View);

END_PCCORE
