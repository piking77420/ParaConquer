#include "resources/material.hpp"

#include "rendering/render_harware_interface/RHI.hpp"
#include "app.hpp"

#include <utility>
using namespace PC_CORE;




void Material::WriteFile(const fs::path& path)
{
    Resource::WriteFile(path);
}


Material::Material(Material&& _other) noexcept : Resource(std::move(_other))
, albedo(std::exchange(_other.albedo, nullptr)) 
, color(std::move(_other.color))
, roughness(_other.roughness)
, metallic(_other.metallic)
, anisotropy(_other.anisotropy)
, emmisive(std::move(_other.emmisive))
, ambiantOcculusion(_other.ambiantOcculusion)
, descriptorSetHandle(std::move(_other.descriptorSetHandle)) 
, m_Shader(std::exchange(_other.m_Shader, nullptr))
{
    std::exchange(descriptorSetHandle, _other.descriptorSetHandle);
}

Material::Material(const fs::path& _path) : Resource(_path)
{
    if (m_Shader == nullptr)
        return;

    RHI::GetInstance().AllocDescriptorSet(m_Shader->GetName(), 1, descriptorSetHandle.data(), static_cast<uint32_t>(descriptorSetHandle.size()));
}


Material::Material(const std::string _materialName, const PC_CORE::ShaderProgram& _shader) : m_Shader(&_shader)
{
    // TO DO REWORK RESOURCE
    path = _materialName;
    name = _materialName;

    RHI::GetInstance().AllocDescriptorSet(m_Shader->GetName(), 1, descriptorSetHandle.data(), static_cast<uint32_t>(descriptorSetHandle.size()));
}

Material::~Material()
{
    if (m_Shader == nullptr)
        return;

    RHI::GetInstance().FreeDescriptorSet(m_Shader->GetName(), descriptorSetHandle.data(), static_cast<uint32_t>(descriptorSetHandle.size()));
    m_Shader = nullptr;
}



void Material::Load(std::vector<Texture*> textures)
{
    albedo = textures[0];
}

void Material::Build()
{

    DescriptorImageInfo descriptorImageInfo = albedo->GetDescriptorImageInfo();

    for (size_t i = 0; i < descriptorSetHandle.size(); i++)
    {
        DescriptorWriteSet descriptorWrite =
        {
        .dstDescriptorSetHandle = descriptorSetHandle[i],
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .descriptorBufferInfo = nullptr,
        .descriptorImageInfo = &descriptorImageInfo,
        .descriptorTexelBufferViewInfo = nullptr,
        };

        RHI::GetInstance().UpdateDescriptorSet(1, &descriptorWrite);
    }

}

