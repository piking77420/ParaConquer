#include "resources/material.hpp"

#include "rendering/render_harware_interface/RHI.hpp"
#include "app.hpp"

using namespace PC_CORE;




void Material::WriteFile(const fs::path& path)
{
    Resource::WriteFile(path);
}

Material::Material(const fs::path& _path) : Resource(_path)
{
    RHI::GetInstance().AllocDescriptorSet(App::instance->renderer.shader->name, &descriptorSetHandle, 1);
}

Material::~Material()
{
    RHI::GetInstance().FreeDescriptorSet(App::instance->renderer.shader->name, &descriptorSetHandle, 1);
}



void Material::Load(std::vector<Texture*> textures)
{
    albedo = textures[0];
}

void Material::BuildDescriptor()
{
    DescriptorImageInfo descriptorImageInfo = albedo->GetDescriptorImageInfo();
    DescriptorWriteSet descriptorWrite =
        {
        .dstDescriptorSetHandle = descriptorSetHandle,
        .dstBinding = 3,
        .dstArrayElement = 0,
        .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .descriptorBufferInfo = nullptr,
        .descriptorImageInfo = &descriptorImageInfo,
        .descriptorTexelBufferViewInfo = nullptr,
        };

    RHI::GetInstance().UpdateDescriptorSet(1, &descriptorWrite);
}
