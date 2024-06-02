#pragma once

#include "core_header.hpp"
#include "resource.hpp"

BEGIN_PCCORE
class Texture : public IResource
{
public:
    
    
    ~Texture() override;
    
    void Load(const fs::path& path) override;
};

END_PCCORE