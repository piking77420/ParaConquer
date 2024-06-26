#pragma once

#include "core_header.hpp"
#include "texture.hpp"

BEGIN_PCCORE
class Material : public IResource
{
public:
    void WriteFile(const fs::path& path) override;

    ~Material() override = default;
    
    void Load(const fs::path& path) override;

    void Load(std::vector<Texture*> textures);

    Texture* albedo = nullptr;

    Vector3f color = {0.5f, 0.5f, 0.5f};

    float metallic = 0.f;
    
    float roughness = 0.f;
    
    float anisotropy = 0.f;
    
    Vector3f emmisive = {};
    
    float ambiantOcculusion = 0.f;
};

END_PCCORE