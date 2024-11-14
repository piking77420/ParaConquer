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
    RHI::GetInstance().AllocDescriptorSet(App::instance->renderer.shader->name,1 , descriptorSetHandle.data(), static_cast<uint32_t>(descriptorSetHandle.size()));
}


Material::Material()
{
    RHI::GetInstance().AllocDescriptorSet(App::instance->renderer.shader->name,1 , descriptorSetHandle.data(), static_cast<uint32_t>(descriptorSetHandle.size()));
}

Material::~Material()
{
    RHI::GetInstance().FreeDescriptorSet(App::instance->renderer.shader->name, descriptorSetHandle.data(), static_cast<uint32_t>(descriptorSetHandle.size()));
}



void Material::Load(std::vector<Texture*> textures)
{
    albedo = textures[0];
}

void Material::BuildDescriptor()
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
