#pragma once

#include "core_header.hpp"
#include "texture.hpp"

BEGIN_PCCORE
class Material : public IResource
{
public:
    Texture* albedo = nullptr;

    Tbx::Vector3f color = {0.5f, 0.5f, 0.5f};

    float roughness = 0.f;
    
    float metallic = 0.f;
    
    float anisotropy = 0.f;
    
    Tbx::Vector3f emmisive = {};
    
    float ambiantOcculusion = 0.f;
    
    void WriteFile(const fs::path& path) override;

    ~Material() override;
    
    void Load(const fs::path& path) override;

    void Load(std::vector<Texture*> textures);

};

END_PCCORE