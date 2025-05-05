#include "rendering/material.hpp"

#include "app.hpp"


PC_CORE::Material::Material()
{
}


PC_CORE::Material::Material(const std::string& _name)
{
    name = _name;
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
    
    if (!m_ShaderProgram.expired())
        m_ShaderProgram.lock()->AllocDescriptorSet(&m_pShaderProgramDescriptorSets, 1);
}

PC_CORE::Material::~Material()
{
    // TO DO HANDLE RESOURCES INTRA DEPENDANCIES
    if (!m_ShaderProgram.expired())
        m_ShaderProgram.lock()->FreeDescriptorSet(&m_pShaderProgramDescriptorSets);
}

void PC_CORE::Material::Build()
{
    if (m_albedo.expired())
    {
        PC_LOGERROR("albedo texture expired");
        return;
    }


    ImageSamperDescriptor imageSamperDescriptor =
       {
        .sampler = Rhi::GetRhiContext()->sampler.get(),
        .texture = m_albedo.lock().get()
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
