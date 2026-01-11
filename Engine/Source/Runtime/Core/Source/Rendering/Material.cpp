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

    m_RhiDescriptorSets.reset(App::Instance->RenderHarwareInteface.CreateDescriptorSet());
    m_RhiDescriptorSets->BindUniformBuffer(RhiShaderStageBits::Pixel, 0, m_RhiMaterialBuffer.get());

    for (size_t i = 0; i < m_Textures.size(); i++)
    {
        if (auto tex = m_Textures[i].lock())
        {
            m_RhiDescriptorSets
                ->BindTexture(RhiShaderStageBits::Pixel, static_cast<uint32_t>(i), tex.get()->Get(), App::Instance->TextureSampler.get());
        }
    }

    m_RhiDescriptorSets->SetName(Name + "DescriptorSet")
        .Build();
}

RhiDescriptorSet* Material::GetDescriptorSet()
{
    return m_RhiDescriptorSets.get();
}

}
