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


PC_CORE::Material::Material(const std::string& _name) 
    : Resource(_name)
{
    DYNAMIC_REFLECT_INIT

    switch (MaterialType)
    {
    case MaterialType::Opaque:
        m_ShaderProgram = App::Instance->Renderer.GeometryBufferShader.get();
        break;
    case MaterialType::Transparent:
        m_ShaderProgram = App::Instance->Renderer.ForwardShader.get();
        break;
    default: ;
    }

    if (m_ShaderProgram && Albedo.Lock())
    {
        m_PShaderProgramDescriptorSets.reset(m_ShaderProgram->CreateDescriptorBinding());

        ImageSamplerDescriptor imageSamperDescriptor =
        {
            .sampler = ResourceManager::Get<Sampler>(std::string("LinearRepeat"))->Get(),
            .texture = Albedo.Lock()->Get(),
            .resourceState = RhiResourceState::FragmentShaderResource
        };

        std::vector<DescriptorWrite> DescriptorSets =
        {
            {
                DescriptorType::CombinedImageSampler,
                ALBEDO_BINDING,
                imageSamperDescriptor,
            },
        };

        m_PShaderProgramDescriptorSets
            ->SetBindings(MATERIAL_DESCRIPTOR_SET, DescriptorSets)
            .SetName(std::format("Material Binding {}", Name))
            .Build();
    }
}

PC_CORE::Material::~Material()
{

}