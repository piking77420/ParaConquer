#pragma once

#include "CoreHeader.hpp"
#include "Resource.hpp"
#include "Texture.hpp"
#include "LowRenderer/RhiTexure2d.hpp"
#include "Math/ToolboxTypedef.hpp"
#include "Reflection/Reflector.hpp"
#include "LowRenderer/RhiTypedef.h"
#include "Rendering/Buffer/GpuBuffer.hpp"


BEGIN_PCCORE
class Texture2D final : public Texture
{
public:
  
    PC_CORE_API IMP_DYNAMIC_REFLECT()
    
    DEFAULT_COPY_MOVE_OPERATIONS(Texture2D)
        
    PC_CORE_API RHIFormat GetRHIFormat() const;

    std::shared_ptr<RhiResource> GetRhiHandle() const override
    {
        return m_Texture2D;
    }

    std::shared_ptr<RhiTexture2D> GetRhiTexture2D()
    {
        return m_Texture2D;
    }

    const std::shared_ptr<RhiTexture2D> GetRhiTexture2D() const
    {
        return m_Texture2D;
    }

    Tbx::Vector2i GetSize() const
    {
        return m_Size;
    }

    RHIFormat GetFormat()
    {
        return m_Texture2D ? GetFormat() : RHIFormat::UNDEFINED;
    }
    
    PC_CORE_API Texture2D();

    PC_CORE_API Texture2D(const std::string& _name);

    PC_CORE_API Texture2D(const std::string& _name, const std::string& _path);

    PC_CORE_API Texture2D(const CreateImageInfo& createTextureInfo);
    
    PC_CORE_API ~Texture2D() override;
    
private:

    std::shared_ptr<RhiTexture2D> m_Texture2D;
    
    RHIFormat m_Format;

    Tbx::Vector2i m_Size;

    std::filesystem::path m_TexturePath;

    void LoadTextureFromPath(const std::string& _path);

};

REFLECT(Texture2D, Resource)


END_PCCORE