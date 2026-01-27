#pragma once

#include "Ecs/EcsFront.h"
#include "Resources/StaticMesh.hpp"
#include "Rendering/Material.hpp"
#include "Objectptr.hpp"

BEGIN_PCCORE
    struct StaticMeshComponent : Component
    {
        WeakObjectPtr<StaticMesh> staticMesh;

        std::vector<WeakObjectPtr<Rendering::Material>> materials;

        bool UseMeshlet = false;
    };

    REFLECT(StaticMeshComponent, Component)
    REFLECT_MEMBER(StaticMeshComponent, staticMesh)
    REFLECT_MEMBER(StaticMeshComponent, materials)
    REFLECT_MEMBER(StaticMeshComponent, UseMeshlet)

END_PCCORE
