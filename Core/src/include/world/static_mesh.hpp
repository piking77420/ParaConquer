#pragma once

#include "core_header.hpp"
#include "resources/material.hpp"
#include "resources/mesh.hpp"
#include "ecs/ecs_front.h"

BEGIN_PCCORE
struct StaticMesh : public Component
{

    Mesh* mesh = nullptr;

    MaterialInstance* materialInstance  = nullptr ;
    
};
REFLECT(StaticMesh, Component)

END_PCCORE