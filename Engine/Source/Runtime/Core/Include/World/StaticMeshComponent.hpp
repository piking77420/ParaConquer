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

#ifdef WITH_EDITOR
        bool ShowBound;
#endif
    };

    REFLECT(StaticMeshComponent, Component)
    REFLECT_MEMBER(StaticMeshComponent, staticMesh)
    REFLECT_MEMBER(StaticMeshComponent, materials)
#ifdef WITH_EDITOR
    REFLECT_MEMBER(StaticMeshComponent, ShowBound)
#endif

END_PCCORE
