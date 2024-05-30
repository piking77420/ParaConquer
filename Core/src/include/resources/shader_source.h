#pragma once
#include "resource.hpp"


BEGIN_PCCORE

class ShaderSource : public IResource
{
public:
    enum class ShaderType 
    {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESSELATION,
        COMPUTE,

        SHADERTYPEMAX
    };

    inline static std::array<std::string, 5> ShaderSourceFormat
    {
      ".vertex"
      ".fragment"
      ".geometry"
      ".tesselation"
      ".compute"
    };
    
    ShaderType shaderType;
    
    uint8_t* data = nullptr;
    
    void Load(const fs::path& _path) override;
};

END_PCCORE