#pragma once

#include "Rendering/RenderingTypedef.h"
#include "Math/ToolBoxTypeDef.hpp"
#include "Rendering/RenderSystem.hpp"
#include <Frustum.hpp>

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

            mat4 FrustumViewMatrix; // psp

            vec4 FrustumPlanesView[6];

            float CameraNear;
            float CameraFar;
            float DeltaTime;
            float Gamma;

            float Exposure;
            vec2 RenderSize;
            float pad00;

            vec2 InvRenderSize;
            uint32_t MeshletCulling;
            float pad01;
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
            DrawFrustum = 1 << 1,
            DrawBounds = 1 << 2
        };
        using RenderViewFlag = uint8_t;


        void FromCamera(const PC_CORE::Camera& _Camera);

        void SetRenderSize(Tbx::Vector2f _Size)
        {
            RenderSize = _Size;
            InvRenderSize = Tbx::Vector2f(1.f /RenderSize.x, 1.f /RenderSize.y);
        }

        void UpdaterRhiBuffers(CommandList& cmd, const PC_CORE::Rendering::RenderingWorldData& _RenderingWorldData);

        Tbx::Matrix4x4d View;
        Tbx::Matrix4x4d ViewInv;

        Tbx::Matrix4x4d Projection;
        Tbx::Matrix4x4d ProjectionNative;
        Tbx::Matrix4x4d ProjectionInv;

        Tbx::Matrix4x4d ViewProjection;
        Tbx::Matrix4x4d ViewProjectionInv;

        Tbx::Matrix4x4d FrustumToView;
        Tbx::Matrix4x4d FrustumViewMatrix;
        Tbx::Matrix4x4d FrustumToWorld;

        Frustum FrustumView;
        Frustum FrustumWorld;

        double CameraNear;
        double CameraFar;

        double Deltatime;

        double Gamma;
        double Exposure;

        Tbx::Vector2f RenderSize{};
        Tbx::Vector2f InvRenderSize{};

        RenderViewFlag Flag{};

        bool MeshletCulling = false;

        std::unique_ptr<RhiBuffer> UniformBuffer;

        std::unique_ptr<RhiBuffer> LightBuffer;

        std::unique_ptr<RhiBuffer> LightBufferHeader;

        Tbx::Vector3d ViewPosition;

        double Fov;
    };
}

