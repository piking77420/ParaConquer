#include "rendering/material.hpp"

#include "App.hpp"
#include "Rendering/Sampler.hpp"
#include "Resources/ResourceManager.hpp"


std::shared_ptr<PC_CORE::MaterialInstance> PC_CORE::Material::CreateMaterialInstance()
{
    return nullptr;
}

PC_CORE::Material::Material()
{
    DYNAMIC_REFLECT_INIT
}


PC_CORE::Material::Material(const std::string& _name) : Resource(_name)
{
    DYNAMIC_REFLECT_INIT
    
    switch (materialType)
    {
    case MaterialType::Opaque:
        m_ShaderProgram = App::instance->renderer.m_GeometryBufferShader;
        break;
    case MaterialType::Transparent:
        m_ShaderProgram = App::instance->renderer.m_ForwardShader;
        break;
    default: ;
    }
    
    
    if (!m_ShaderProgram.expired())
        m_ShaderProgram.lock()->AllocDescriptorSet(&m_PShaderProgramDescriptorSets, MATERIAL_DESCRIPTOR_SET);
}

PC_CORE::Material::~Material()
{
   
    if (!m_ShaderProgram.expired())
        m_ShaderProgram.lock()->FreeDescriptorSet(&m_PShaderProgramDescriptorSets);
}

void PC_CORE::Material::Build()
{
    if (albedo.expired())
    {
        PC_LOGERROR("albedo texture expired");
        return;
    }


    ImageSamplerDescriptor imageSamperDescriptor =
    {
        .sampler = ResourceManager::Get<Sampler>("LinearRepeat").get(),
        .texture = albedo.lock().get(),
        .imageState = PC_CORE::ImageState::ShaderReadOptimal
    };

    
    std::vector<PC_CORE::ShaderProgramDescriptorWrite> descriptorSets =
   {
        {
            ShaderProgramDescriptorType::CombinedImageSampler,
            ALBEDO_BINDING,
            imageSamperDescriptor,
        },
   };

   m_PShaderProgramDescriptorSets->WriteDescriptorSets(descriptorSets);
}

