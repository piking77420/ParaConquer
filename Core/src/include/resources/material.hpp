#pragma once

#include "core_header.hpp"
#include "texture.hpp"

BEGIN_PCCORE
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

    PC_CORE_API ~Material() override;
    
    PC_CORE_API void Load(std::vector<Texture*> textures);

};

END_PCCORE