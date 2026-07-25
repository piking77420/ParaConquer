#pragma once

#include "CoreHeader.hpp"
#include "Texture.hpp"

BEGIN_PCCORE
    class PC_CORE_API Texture3D : public Texture
    {
    public:
        Texture3D() : Texture()
        {
            DYNAMIC_REFLECT_INIT
        }

        Texture3D(PC_CORE::Rhi& rhi, const std::string& _name, const std::array<std::string, 6>& _maps);

        ~Texture3D() override = default;

        Texture3D(Texture3D&&) noexcept = default;
        
        Texture3D& operator=(Texture3D&&) noexcept = default;

        IMP_DYNAMIC_REFLECT()


    protected:
        std::vector<std::filesystem::path> m_FileSources;

        REFLECT(Texture3D, Texture)
        REFLECT_MEMBER(Texture3D, m_FileSources)
    };

END_PCCORE
