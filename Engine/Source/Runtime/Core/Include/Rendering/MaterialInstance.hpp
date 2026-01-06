#pragma once

#include "CoreHeader.hpp"

BEGIN_PCCORE
    class Material;

    class MaterialInstance
    {
        friend Material;

        Material* m_pMaterial = nullptr;
    };

END_PCCORE
