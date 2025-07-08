#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "rendering/gpu_resource.hpp"

BEGIN_PCCORE
PC_CORE_API class Texture : public Resource , public IGpuResource
{
public:
    
     IMP_DYNAMIC_REFLECT()

     Texture()
    {
        DYNAMIC_REFLECT_INIT;
    }

     explicit Texture(const fs::path& _path) : Resource(_path) 
     {
         DYNAMIC_REFLECT_INIT;
     }

     ~Texture() = default;
};

REFLECT(Texture, Resource)


END_PCCORE