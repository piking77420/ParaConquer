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
        if (auto Text = m_Textures[i].Lock())
        {
            m_RhiDescriptorSets
                ->BindTexture(RhiShaderStageBits::Pixel, static_cast<uint32_t>(i) + 1, Text->Get(), App::Instance->TextureSampler.get());
        }
        else
        {
            m_RhiDescriptorSets
                ->BindTexture(RhiShaderStageBits::Pixel, static_cast<uint32_t>(i) + 1, App::Instance->DummyTexture.get(), App::Instance->TextureSampler.get());
        }
        
    }
    m_RhiDescriptorSets->SetName(Name + "DescriptorSet")
        .Build();

    Upload();
}


void Material::Upload()
{
    Gpu::MaterialBuffer MaterialBufferData;
    PopulateGpuMaterial(MaterialBufferData);

    if (char* ptr = m_RhiMaterialBuffer->BeginFullDynamicBufferUpdateForCurrentFrame())
    {
        assert(m_RhiMaterialBuffer->GetStride() == m_RhiMaterialBuffer->GetFrameStride());
        for (size_t i = 0; i < MaxFramesInFlight; i++)
        {
            Gpu::MaterialBuffer* materialGpu = reinterpret_cast<Gpu::MaterialBuffer*>(ptr + (i * m_RhiMaterialBuffer->GetFrameStride()));
            *materialGpu = MaterialBufferData;
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

void Material::PopulateGpuMaterial(Gpu::MaterialBuffer& _MaterialBuffer)
{
    for (size_t i = 0; i < m_Textures.size(); i++)
    {
        if (auto Text = m_Textures[i].Lock())
        {
            _MaterialBuffer.TextureDescriptor[i] = 1;
        }
        else
        {
            const MaterialAttribute att = static_cast<MaterialAttribute>(i);
            switch (att)
            {
			case MaterialAttribute::Albedo:
                std::memcpy(_MaterialBuffer.Albedo.data.data(), &m_Albedo.x, sizeof(_MaterialBuffer.Albedo.data));
				break;
            case MaterialAttribute::MetallicRoughnessAnisotropy:
            {
                std::array<float, 4> buffer;
                buffer[0] = m_Metallic;
                buffer[1] = m_Specular;
                buffer[2] = m_Roughness;
                buffer[3] = m_Anisotropy;
                std::memcpy(_MaterialBuffer.MetallicRoughnessAnisotropy.data.data(), &buffer, sizeof(_MaterialBuffer.MetallicRoughnessAnisotropy.data));
            }
				break;
			case MaterialAttribute::Normal:
				break;
			case MaterialAttribute::Emisive:
                std::memcpy(_MaterialBuffer.Emissive.data.data(), &m_Emmisive, sizeof(_MaterialBuffer.Emissive.data));
				break;
			case MaterialAttribute::Ao:
                _MaterialBuffer.AO = m_AmbiantOcclusion;
				break;

            default:
                break;
            }
        }
    }


}

}
