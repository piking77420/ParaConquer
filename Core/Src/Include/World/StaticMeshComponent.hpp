#pragma once

#include "CoreHeader.hpp"
#include "Resources/Mesh.hpp"
#include "Ecs/EcsFront.h"
#include "Rendering/Material.hpp"

BEGIN_PCCORE
struct StaticMeshComponent : public Component
{
    ResourceRef<Mesh> mesh;


    ResourceRef<Material> material;
};
REFLECT(StaticMeshComponent, Component)
REFLECT_MEMBER(StaticMeshComponent, mesh)
REFLECT_MEMBER(StaticMeshComponent, material)

END_PCCORE