#include "Rendering/View/View.hpp"

#include "LowRenderer/Rhi.hpp"
#include "Rendering/View/ViewUniformBuffer.hpp"

using namespace PC_CORE::Rendering;

View::View(PC_CORE::Rhi& _Rhi, Tbx::Vector2i _viewPortSize)
    : m_Rhi(_Rhi)
    , renderSize(_viewPortSize)
{
    SceneBufferUbo.reset(_Rhi.CreateBuffer());
    SceneBufferUbo
        ->SetUsage(RhiBuffer::BufferUsageFlagBits::Uniform)
        .SetMemoryUsage(RhiMemoryUsage::Dynamic)
        .SetSize(sizeof(Rendering::Gpu::ViewUniformBuffer))
        .SetName("SceneBufferUniformBuffer")
        .Build();

}
