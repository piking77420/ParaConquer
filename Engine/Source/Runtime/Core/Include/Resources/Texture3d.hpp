#pragma once

#include "CoreHeader.hpp"
#include "Texture.hpp"

BEGIN_PCCORE
    class PC_CORE_API Texture3D : public Texture
    {
    public:
        IMP_DYNAMIC_REFLECT()

        DEFAULT_COPY_MOVE_OPERATIONS(Texture3D)

        Texture3D() : Texture()
        {
            DYNAMIC_REFLECT_INIT
        }

        Texture3D(PC_CORE::Rhi& rhi, const std::string& _name, const std::array<std::string, 6>& _maps);

        ~Texture3D() override = default;

    protected:
        std::vector<std::filesystem::path> m_FileSources;

        REFLECT(Texture3D, Texture)
        REFLECT_MEMBER(Texture3D, m_FileSources)
    };

END_PCCORE
