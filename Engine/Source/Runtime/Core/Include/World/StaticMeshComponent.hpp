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

        bool ShowDrawCommandsBounds;
#endif
    };

    REFLECT(StaticMeshComponent, Component)
    REFLECT_MEMBER(StaticMeshComponent, staticMesh)
    REFLECT_MEMBER(StaticMeshComponent, materials)
#ifdef WITH_EDITOR
    REFLECT_MEMBER(StaticMeshComponent, ShowBound)
    REFLECT_MEMBER(StaticMeshComponent, ShowDrawCommandsBounds)
#endif

END_PCCORE
