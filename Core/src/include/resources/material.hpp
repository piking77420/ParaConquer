#pragma once

#include "core_header.hpp"
#include "texture.hpp"

BEGIN_PCCORE
class Material : public IResource
{
public:
    ~Material() override = default;
    
    void Load(const fs::path& path) override;

    Texture* albedo = nullptr;
};

END_PCCORE