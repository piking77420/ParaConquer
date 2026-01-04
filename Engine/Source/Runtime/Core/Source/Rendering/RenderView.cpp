#include "Rendering/RenderView.hpp"

#include "Rendering/Camera.hpp"
#include "LowRenderer/Rhi.hpp"


namespace PC_CORE::Rendering  
{
    RenderView::RenderView(PC_CORE::Rhi& _Rhi)
    {
        UniformBuffer.reset(_Rhi.CreateBuffer());
        UniformBuffer
            ->SetMemoryUsage(RhiMemoryUsage::Dynamic)
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
        CameraFar = _Camera.GetNear();
	}

    void RenderView::UpdateUniformBuffer()
    {
        assert(UniformBuffer);

        if (Gpu::RenderViewViewUniformBuffer* ptr = reinterpret_cast<Gpu::RenderViewViewUniformBuffer*>(UniformBuffer->BeginFullDynamicBufferUpdateForCurrentFrame()))
        {
            std::memcpy(ptr, this, UniformBuffer->GetSize());
            UniformBuffer->EndFullDynamicBufferUpdateForCurrentFrame();
        }
    }

} // PC_CORE::Rendering

