#pragma once

#include "core_header.hpp"
#include "resources/material.hpp"
#include "resources/mesh.hpp"
#include "ecs/ecs_front.h"

BEGIN_PCCORE
MAKE_ECS_COMPONENT(StaticMesh)

    Mesh* mesh = nullptr;

    Material* material  = nullptr ;
    
};
REFLECT(StaticMesh, Component)

END_PCCORE