#pragma once

#include "core_header.hpp"
#include "resources/mesh.hpp"
#include "ecs/ecs_front.h"
#include "rendering/material.hpp"

BEGIN_PCCORE
struct StaticMesh : public Component
{
    ResourceRef<Mesh> mesh;


    ResourceRef<Material> material;
};
REFLECT(StaticMesh, Component)
REFLECT_MEMBER(StaticMesh, mesh)
REFLECT_MEMBER(StaticMesh, material)

END_PCCORE