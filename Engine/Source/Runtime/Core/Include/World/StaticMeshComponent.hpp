#pragma once

#include "Ecs/EcsFront.h"
#include "Resources/StaticMesh.hpp"
#include "Rendering/Material.hpp"
#include "Objectptr.hpp"

BEGIN_PCCORE
    struct StaticMeshComponent : Component
    {
        WeakObjectPtr<StaticMesh> staticMesh;

        WeakObjectPtr<Rendering::Material> material;
    };

    REFLECT(StaticMeshComponent, Component)
    REFLECT_MEMBER(StaticMeshComponent, staticMesh)
    REFLECT_MEMBER(StaticMeshComponent, material)

END_PCCORE
