#include "rendering/material.hpp"

#include "app.hpp"
#include "rendering/sampler.hpp"
#include "resources/resource_manager.hpp"


PC_CORE::Material::Material()
{
    DYNAMIC_REFLECT_INIT
}


PC_CORE::Material::Material(const std::string& _name)
{
    DYNAMIC_REFLECT_INIT
    
    name = _name;
    switch (materialType)
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
        .sampler = ResourceManager::Get<Sampler>("LinearRepeat").get(),
        .texture = m_albedo.lock().get()
    };

    
    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets =
   {
        {
            ShaderProgramDescriptorType::CombineImageSampler,
            ALBEDO_BINDING,
            nullptr,
            &imageSamperDescriptor,
        },
   };

   m_pShaderProgramDescriptorSets->WriteDescriptorSets(descriptorSets);
}

