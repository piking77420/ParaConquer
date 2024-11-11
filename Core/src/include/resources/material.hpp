#pragma once

#include "core_header.hpp"
#include "texture.hpp"
#include "rendering/render_harware_interface/descriptor_set.hpp"

#define MATERIAL_SET 1

BEGIN_PCCORE

// TO DO HANDLE DYNAMIC UNIFORM BUFFER

class Material : public Resource
{
public:
    Texture* albedo = nullptr;

    Tbx::Vector3f color = {0.5f, 0.5f, 0.5f};

    float roughness = 0.f;
    
    float metallic = 0.f;
    
    float anisotropy = 0.f;
    
    Tbx::Vector3f emmisive = {};
    
    float ambiantOcculusion = 0.f;
    
    PC_CORE_API void WriteFile(const fs::path& _path) override;

    PC_CORE_API Material(const fs::path& _path);

    PC_CORE_API Material();

    PC_CORE_API ~Material() override;
    
    PC_CORE_API void Load(std::vector<Texture*> textures);

    PC_CORE_API void BuildDescriptor();
    

    std::array<DescriptorSetHandle, MAX_FRAMES_IN_FLIGHT> descriptorSetHandle;

};

END_PCCORE