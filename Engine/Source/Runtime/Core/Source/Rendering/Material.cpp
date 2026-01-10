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
    
    if (auto text = m_Albedo.Lock())
    {
        PC_LOG_VERBOSE("Build Material Name {}", Name);

        TextureSampler.reset(App::Instance->RenderHarwareInteface.CreateSampler());
        TextureSampler
            ->SetMagFilter(Filter::Linear)
            .SetMinFilter(Filter::Linear)
            .SetU(SamplerAddressMode::Repeat)
            .SetV(SamplerAddressMode::Repeat)
            .SetW(SamplerAddressMode::Repeat)
            .SetName("TextureSampler")
            .Build();



        m_RhiDescriptorSets.reset(App::Instance->RenderHarwareInteface.CreateDescriptorSet());
        m_RhiDescriptorSets
            ->BindTexture(RhiShaderStageBits::Pixel, 0, text->Get(), TextureSampler.get())
            .SetName(Name + "DescriptorSet")
            .Build();
    }

}

RhiDescriptorSet* Material::GetDescriptorSet()
{
    return m_RhiDescriptorSets.get();
}

}
