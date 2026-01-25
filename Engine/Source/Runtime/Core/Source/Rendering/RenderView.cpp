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
            .SetSize(sizeof(Gpu::RenderViewViewUniformBuffer))
            .SetName("RenderViewUniformBuffer")
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

        CameraNear = _Camera.GetNear();
        CameraFar = _Camera.GetFar();


        ViewPosition = _Camera.Position;
	}

    void RenderView::UpdateUniformBuffer()
    {
        assert(UniformBuffer);
        PERF_REGION_SCOPED;
        PERF_REGION_COLOR(PerfRegion::Rendering);

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
    }

} // PC_CORE::Rendering

