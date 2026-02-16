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
        Gamma = 2.2f;
        Exposure = 1.f;

        CameraNear = _Camera.GetNear();
        CameraFar = _Camera.GetFar();


        ViewPosition = _Camera.Position;
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

            Gpu::StreamDoubleToFloat(&ptr->ViewProjection, &ViewProjection);
            Gpu::StreamDoubleToFloat(&ptr->ViewProjectionInv, &ViewProjectionInv);

            ptr->CameraNear = static_cast<float>(CameraNear);
            ptr->CameraFar = static_cast<float>(CameraFar);

            ptr->Deltatime = static_cast<float>(Deltatime);

            ptr->Gamma = static_cast<float>(Gamma);
            ptr->Exposure = static_cast<float>(Exposure);

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
                // shoul be 3x3
                Tbx::Vector4d lightDirV = View * Tbx::Vector4d(_RenderingWorldData.DirLightData->LightDirW.x, _RenderingWorldData.DirLightData->LightDirW.y, _RenderingWorldData.DirLightData->LightDirW.z, 0.0);
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

