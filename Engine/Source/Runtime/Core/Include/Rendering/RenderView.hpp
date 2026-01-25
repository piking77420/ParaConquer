#pragma once

#include "Rendering/RenderingTypedef.h"
#include "Math/ToolBoxTypeDef.hpp"

namespace PC_CORE
{
    class Camera;
    class RhiBuffer;
    class Rhi;
}

namespace PC_CORE::Rendering
{
    namespace Gpu
    {
        struct GPU_ALIGN RenderViewViewUniformBuffer
        {
            mat4 View;
            mat4 ViewInv;

            mat4 Projection;
            mat4 ProjectionInv;

            mat4 ViewProjection;
            mat4 ViewProjectionInv;

            float CameraNear;
            float CameraFar;

            float Deltatime;

            float Gamma;
            float Exposure;

            vec2 RenderSize;
            vec2 InvRenderSize;
        };
    }

    class PC_CORE_API RenderView
    {
    public:
        explicit RenderView(PC_CORE::Rhi& _Rhi);

        ~RenderView() = default;

        DEFAULT_COPY_MOVE_OPERATIONS(RenderView);

        enum RenderViewFlagBits : uint8_t
        {
            DebugGeometry = 1 << 0,
        };
        using RenderViewFlag = uint8_t;


        void FromCamera(const PC_CORE::Camera& _Camera);

        void SetRenderSize(Tbx::Vector2f _Size)
        {
            RenderSize = _Size;
            InvRenderSize = Tbx::Vector2f(1.f /RenderSize.x, 1.f /RenderSize.y);
        }

        void UpdateUniformBuffer();

        Tbx::Matrix4x4d View;
        Tbx::Matrix4x4d ViewInv;

        Tbx::Matrix4x4d Projection;
        Tbx::Matrix4x4d ProjectionInv;

        Tbx::Matrix4x4d ViewProjection;
        Tbx::Matrix4x4d ViewProjectionInv;

        double CameraNear;
        double CameraFar;

        double Deltatime;

        double Gamma;
        double Exposure;

        Tbx::Vector2f RenderSize{};
        Tbx::Vector2f InvRenderSize{};

        RenderViewFlag Flag{};

        std::unique_ptr<RhiBuffer> UniformBuffer;

        Tbx::Vector3d ViewPosition;
    };
}

