#include "rendering/material.hpp"

#include "app.hpp"


PC_CORE::Material::Material()
{
    switch (m_MaterialType)
    {
    case MaterialType::Opaque:
        m_ShaderProgram = App::instance->renderer.m_ForwardShader;
        break;
    case MaterialType::Transparent:
        //m_ShaderProgram = App::instance->renderer.m_ForwardShader;
        break;
    default: ;
    }
    
    m_ShaderProgram->AllocDescriptorSet(&m_pShaderProgramDescriptorSets, 1);
}

PC_CORE::Material::~Material()
{
    m_ShaderProgram->FreeDescriptorSet(&m_pShaderProgramDescriptorSets);
}

void PC_CORE::Material::Build()
{
    ImageSamperDescriptor imageSamperDescriptor =
       {
        .sampler = Rhi::GetRhiContext()->sampler.get(),
        .texture = m_albedo
    };

    
    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets =
   {
        {
            ShaderProgramDescriptorType::CombineImageSampler,
            2,
            nullptr,
            &imageSamperDescriptor,
        },
   };

    m_pShaderProgramDescriptorSets->WriteDescriptorSets(descriptorSets);
}

const PC_CORE::ShaderProgramDescriptorSets* PC_CORE::Material::GetDescriptorSet()
{
    return m_pShaderProgramDescriptorSets;
}
