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
    
    PC_CORE_API void SetPath(const fs::path& _path) override;

    std::vector<uint8_t> GetData() override;
    
    void WriteFile(const fs::path& path) override;

    PC_CORE_API ~ShaderSource() override;
    
private:
    std::vector<char> m_DataChar;

    static std::vector<char> ReadFile(const std::string& _filename);
   

END_PCCORE