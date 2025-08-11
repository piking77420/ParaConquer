#pragma once

#include "CoreHeader.hpp"
#include "Resources/Mesh.hpp"
#include "Ecs/EcsFront.h"
#include "Rendering/Material.hpp"

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