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

    static inline  std::array<std::string, 5> ShaderSourceFormat
    {
      ".vert",
      ".frag",
      ".geom",
      ".tess",
      ".comp"
    };
    
    ShaderType shaderType;
    
    std::vector<char> data;
    
    void Load(const fs::path& _path) override;
private:

};

END_PCCORE