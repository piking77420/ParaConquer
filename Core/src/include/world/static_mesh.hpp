#pragma once

#include "component.hpp"
#include "core_header.hpp"
#include "resources/material.hpp"
#include "resources/mesh.hpp"

BEGIN_PCCORE
class StaticMesh
{
public:

    MAKE_COMPONENT(StaticMesh)

    Mesh* mesh = nullptr;

    const Material* material = nullptr ;
    
};

END_PCCORE