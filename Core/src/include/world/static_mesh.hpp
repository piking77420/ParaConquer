#pragma once

#include "core_header.hpp"
#include "resources/mesh.hpp"
#include "ecs/ecs_front.h"
#include "rendering/material.hpp"

BEGIN_PCCORE
    struct StaticMesh : public Component
{

    Mesh* mesh = nullptr;

    Material* material = nullptr;    
};
REFLECT(StaticMesh, Component)

END_PCCORE