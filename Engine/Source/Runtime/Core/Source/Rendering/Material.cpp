#include "rendering/material.hpp"

#include "App.hpp"
#include "LowRenderer/Rhi.hpp"

namespace PC_CORE::Rendering
{

Material::Material()
{
    DYNAMIC_REFLECT_INIT
}

Material::Material(std::string _Name)
    : Resource(std::move(_Name))
{
    DYNAMIC_REFLECT_INIT
    m_MaterialBuffer.Albedo = { 100.f, 0.f, 100.f, 1.f };
}

Material::~Material()
{

}

void Material::Build()
{
    PC_LOG_VERBOSE("Build Material Name {}", Name);
    
    m_RhiMaterialBuffer.reset(App::Instance->RenderHarwareInteface.CreateBuffer());
    m_RhiMaterialBuffer
        ->SetMemoryUsage(RhiMemoryUsage::CPUVisible) // may use static
        .SetBufferUpdateRate(RhiBuffer::BufferUpdateRate::Dynamic)
        .SetUsage(RhiBuffer::BufferUsageFlagBits::Uniform)
        .SetSize(sizeof(Gpu::MaterialBuffer) * MaxFramesInFlight)
        .SetObjectSize(sizeof(Gpu::MaterialBuffer))
        .SetMaxObjectPerFrame(1)
        .SetName(Name + " UniformBuffer")
        .Build();


    // Init Update All Object inside the buffer for each frames
    if (char* ptr = m_RhiMaterialBuffer->BeginFullDynamicBufferUpdateForCurrentFrame())
    {
        assert(m_RhiMaterialBuffer->GetStride() == m_RhiMaterialBuffer->GetFrameStride());

        for (size_t i = 0; i < MaxFramesInFlight; i++)
        {
            Gpu::MaterialBuffer* materialGpu = reinterpret_cast<Gpu::MaterialBuffer*>(ptr + (i * m_RhiMaterialBuffer->GetFrameStride()));
            *materialGpu = m_MaterialBuffer;
        }

        m_RhiMaterialBuffer->EndBufferUpdate();
    }

    m_RhiDescriptorSets.reset(App::Instance->RenderHarwareInteface.CreateDescriptorSet());
    m_RhiDescriptorSets->BindUniformBuffer(RhiShaderStageBits::Pixel, 0, m_RhiMaterialBuffer.get());
    m_RhiDescriptorSets
        ->BindTexture(RhiShaderStageBits::Pixel, 1, m_Textures[0].Lock()->Get(), App::Instance->TextureSampler.get());

    /*for (size_t i = 0; i < m_Textures.size(); i++)
    {
        if (auto tex = m_Textures[i].lock())
        {
            m_RhiDescriptorSets
                ->BindTexture(RhiShaderStageBits::Pixel, static_cast<uint32_t>(i), tex.get()->Get(), App::Instance->TextureSampler.get());
        }
    }*/

    m_RhiDescriptorSets->SetName(Name + "DescriptorSet")
        .Build();
}

Gpu::MaterialBuffer& Material::BeginUpdateMaterialData()
{
    return m_MaterialBuffer;
}

void Material::UpdateMaterialData()
{
    if (char* ptr = m_RhiMaterialBuffer->BeginFullDynamicBufferUpdateForCurrentFrame())
    {
        assert(m_RhiMaterialBuffer->GetStride() == m_RhiMaterialBuffer->GetFrameStride());
        for (size_t i = 0; i < MaxFramesInFlight; i++)
        {
            Gpu::MaterialBuffer* materialGpu = reinterpret_cast<Gpu::MaterialBuffer*>(ptr + (i * m_RhiMaterialBuffer->GetFrameStride()));
            *materialGpu = m_MaterialBuffer;
        }

        m_RhiMaterialBuffer->EndBufferUpdate();
    }
}

const RhiDescriptorSet* Material::GetDescriptorSet() const
{
    return m_RhiDescriptorSets.get();
}

size_t Material::GetMaterialStride() const
{
    return m_RhiMaterialBuffer->GetFrameStride();
}

}
