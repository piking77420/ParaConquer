#include "resources/material_instance.hpp"

#include "app.hpp"
#include "resources/material.hpp"

PC_CORE::MaterialInstance::MaterialInstance(std::string _materialInstanceName, Material* _material) : materialRef(_material)
{
    name = _materialInstanceName;

    _material->CreateMaterialInstance(this);

}

PC_CORE::MaterialInstance::~MaterialInstance()
{
    albedo = nullptr;

    if (materialRef != nullptr)
    {
        materialRef->DestroyMaterialInstace(this);
        materialRef = nullptr;
    }
}

PC_CORE::DescriptorSetHandle PC_CORE::MaterialInstance::GetDescriptorSetHandle() const
{
    if (materialRef == nullptr)
    {
        PC_LOGERROR("PC_CORE::MaterialInstance::GetDescriptorSetHandle is nullptr")
        return nullptr;
    }
    
    return materialRef->GetDesciptorSet(*this);
}

void PC_CORE::MaterialInstance::BuildDescriptorSet()
{
    if (materialRef == nullptr || albedo == nullptr)
    {
        PC_LOGERROR("PC_CORE::MaterialInstance::BuildDescriptorSet() invalid state")
        return;
    }

    // wait device before accessing a resource
    RHI::GetInstance().WaitDevice();

    std::array<DescriptorWriteSet, 1> descriptorWrites;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        DescriptorBufferInfo bufferInfo = App::instance->renderer.renderResources.sceneUniform[i].GetBufferInfo(0);

        DescriptorImageInfo textureInfo =
            {
            .sampler = albedo->GetSamplerHandle(),
            .imageView = albedo->GetImageViewHandle(),
            .imageLayout = LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

        descriptorWrites[0].dstDescriptorSetHandle = nullptr;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].descriptorBufferInfo = nullptr;
        descriptorWrites[0].descriptorImageInfo = &textureInfo;
        descriptorWrites[0].descriptorTexelBufferViewInfo = nullptr;
        
        materialRef->UpdateMaterialInstance(*this, descriptorWrites.data(), static_cast<uint32_t>(descriptorWrites.size()), static_cast<uint32_t>(i));  
    }

    
  
    


  

}
