#pragma once

#include "Ecs/EcsFront.h"
#include "Resources/StaticMesh.hpp"
#include "Rendering/Material.hpp"

BEGIN_PCCORE
struct StaticMeshComponent : public Component
{
    ResourceRef<StaticMesh> staticMesh;

    ResourceRef<Material> material;
};
REFLECT(StaticMeshComponent, Component)
REFLECT_MEMBER(StaticMeshComponent, staticMesh)
REFLECT_MEMBER(StaticMeshComponent, material)

END_PCCORE