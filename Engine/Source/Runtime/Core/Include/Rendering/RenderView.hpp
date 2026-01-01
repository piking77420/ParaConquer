#pragma once

#include "Rendering/RenderingTypedef.h"
#include "Math/ToolBoxTypeDef.hpp"

namespace PC_CORE
{
    class Camera;
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

            float Time;
            float Deltatime;

            float Gamma;
            float Exposure;

            vec2 RenderSize;
            vec2 InvRenderSize;
        };
    }

    struct PC_CORE_API RenderView
    {
    public:
        DEFAULT_CONSTRUCTOR_DESTRUCTOR(RenderView)

        DEFAULT_COPY_MOVE_OPERATIONS(RenderView)

        enum RenderViewFlagBits : uint8_t
        {
            DebugGeometry = 1 << 0,
        };
        using RenderViewFlag = uint8_t;

        void FromCamera(const PC_CORE::Camera& _Camera);

        Tbx::Matrix4x4f View;
        Tbx::Matrix4x4f ViewInv;

        Tbx::Matrix4x4f Projection;
        Tbx::Matrix4x4f ProjectionInv;

        Tbx::Matrix4x4f ViewProjection;
        Tbx::Matrix4x4f ViewProjectionInv;

        float CameraNear;
        float CameraFar;

        float Time;
        float Deltatime;

        float Gamma;
        float Exposure;

        Tbx::Vector2f RenderSize{};
        Tbx::Vector2f InvRenderSize{};

        RenderViewFlag Flag{};

    };
}

