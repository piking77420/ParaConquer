#include "Rendering/RenderView.hpp"

#include "Rendering/Camera.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"


namespace PC_CORE::Rendering  
{
    RenderView::RenderView(PC_CORE::Rhi& _Rhi)
    {
        UniformBuffer.reset(_Rhi.CreateBuffer());
        UniformBuffer
            ->SetMemoryUsage(RhiMemoryUsage::CPUVisible)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::PerFrame)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::Uniform)
            .SetSizeInBytes(sizeof(Gpu::RenderViewViewUniformBuffer))
            .SetName("RenderViewUniformBuffer")
            .Build();

        LightBuffer.reset(_Rhi.CreateBuffer());
        LightBuffer
            ->SetMemoryUsage(RhiMemoryUsage::StaticGPU)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::PerFrame)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::ShaderStorage | RhiBuffer::BufferUsageFlagBits::TransferDst)
            .SetSizeInBytes(sizeof(Gpu::Light) * Gpu::MAX_LIGHT)
            .SetName("Light Buffer")
            .Build();

        LightBufferHeader.reset(_Rhi.CreateBuffer());
        LightBufferHeader
            ->SetMemoryUsage(RhiMemoryUsage::CPUVisible)
            .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::PerFrame)
            .SetUsage(RhiBuffer::BufferUsageFlagBits::Uniform)
            .SetSizeInBytes(sizeof(Gpu::LightHeader))
            .SetName("Light Header Buffer")
            .Build();

        ClipSpaceCorrection = _Rhi.ClipSpaceCorrectionMatrixd();
    }

    void PC_CORE::Rendering::RenderView::FromCamera(const PC_CORE::Camera& _Camera)
	{
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rendering);

        View = _Camera.GetViewMatrix();
        ViewInv = View.Invert();
        Projection = _Camera.GetProjection();
        ProjectionInv = Projection.Invert();
        ViewProjection = _Camera.GetViewProjection();
        ViewProjectionInv = ViewInv * ProjectionInv;

        FrustumView = Frustum(Frustum::OpenglNdc, FrustumToView);
        FrustumWorld = Frustum(Frustum::OpenglNdc, FrustumToWorld);

        Tbx::Vector3f Posf = static_cast<Tbx::Vector3f>(_Camera.Position);
        CameraPos = Tbx::Vector4f(Posf.x, Posf.y, Posf.z, 0.0f);

        Gamma = 2.2f;
        Exposure = 1.f;

        CameraNear = _Camera.GetNear();
        CameraFar = _Camera.GetFar();


        ViewPosition = _Camera.Position;
        Fov = _Camera.GetFov();
	}

    void RenderView::UpdaterRhiBuffers(CommandList& cmd, const PC_CORE::Rendering::RenderingWorldData& _RenderingWorldData)
    {
        assert(UniformBuffer);
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rendering);

        // Update Camera Buffer
        if (Gpu::RenderViewViewUniformBuffer* ptr = reinterpret_cast<Gpu::RenderViewViewUniformBuffer*>(UniformBuffer->BeginBufferUpdateForCurrentFrame()))
        {
            Gpu::StreamDoubleToFloat(&ptr->View, &View);
            Gpu::StreamDoubleToFloat(&ptr->ViewInv, &ViewInv);

            Gpu::StreamDoubleToFloat(&ptr->Projection, &Projection);
            Gpu::StreamDoubleToFloat(&ptr->ProjectionInv, &ProjectionInv);
            Gpu::StreamDoubleToFloat(&ptr->ClipSpaceCorrection, &ClipSpaceCorrection);

            Gpu::StreamDoubleToFloat(&ptr->ViewProjection, &ViewProjection);
            Gpu::StreamDoubleToFloat(&ptr->ViewProjectionInv, &ViewProjectionInv);
            Gpu::StreamDoubleToFloat(&ptr->FrustumViewMatrix, &FrustumViewMatrix);

            FrustumView.StreamPlanes(ptr->FrustumPlanesView[0].data.data());
            std::memcpy(&ptr->CameraPos, &CameraPos.x, sizeof(float) * 4);

            ptr->CameraNear = static_cast<float>(CameraNear);
            ptr->CameraFar = static_cast<float>(CameraFar);

            ptr->DeltaTime = static_cast<float>(Deltatime);

            ptr->Gamma = static_cast<float>(Gamma);
            ptr->Exposure = static_cast<float>(Exposure);
            ptr->MeshletCulling = MeshletCulling;
            ptr->isYUpFrameBuffer = UniformBuffer->GetRhi().IsYUpFrameBuffer();
            ptr->isYUpNdc = UniformBuffer->GetRhi().IsYUpNdc();

            std::memcpy(&ptr->RenderSize, &RenderSize, 2 * sizeof(float));
            std::memcpy(&ptr->InvRenderSize, &InvRenderSize, 2 * sizeof(float));

            UniformBuffer->EndBufferUpdate();
        }

        // Update Light Header
        if (Gpu::LightHeader* ptr = reinterpret_cast<Gpu::LightHeader*>(LightBufferHeader->BeginBufferUpdateForCurrentFrame()))
        {
            ptr->LightCount = static_cast<uint32_t>(_RenderingWorldData.LightsData.size());
            if (_RenderingWorldData.DirLightData)
            {
                Tbx::Vector4d lightDirV = Tbx::Vector4d(_RenderingWorldData.DirLightData->LightDirW.x, _RenderingWorldData.DirLightData->LightDirW.y, _RenderingWorldData.DirLightData->LightDirW.z, 0.0);
                lightDirV = lightDirV.Normalize();

                ptr->DirLight.Direction = { static_cast<float>(lightDirV.x) ,static_cast<float>(lightDirV.y),static_cast<float>(lightDirV.z) };
                

                ptr->DirLight.ColorIntensity.data[0] = _RenderingWorldData.DirLightData->LightColor.x;
                ptr->DirLight.ColorIntensity.data[1] = _RenderingWorldData.DirLightData->LightColor.y;
                ptr->DirLight.ColorIntensity.data[2] = _RenderingWorldData.DirLightData->LightColor.y;
                ptr->DirLight.Pad0 = 0xDEAD;
                ptr->DirLight.ColorIntensity.data[3] = _RenderingWorldData.DirLightData->LightIntensity;

            }
            else
            {
                std::memset(&ptr->DirLight, 0, sizeof(ptr->DirLight));
            }
            LightBufferHeader->EndBufferUpdate();
        }
   
    }

} // PC_CORE::Rendering

