#pragma once

#include "core_header.hpp"

BEGIN_PCCORE

class Material;

class MaterialInstance
{
public:

private:
    friend Material;

    Material* m_pMaterial = nullptr;
};

END_PCCORE