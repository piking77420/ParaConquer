#pragma once

#include "rendering_typedef.h"
#include "ecs/ecs_system.h"

BEGIN_PCCORE

class Level;

class RendererSystem : public EcsSystem
{

public:

    DEFAULT_COPY_MOVE_OPERATIONS(RendererSystem)
       
    IMP_DYNAMIC_REFLECT();

    PC_CORE_API void Begin() override {};

    PC_CORE_API void Tick(double deltaTime) override {};

    PC_CORE_API void RenderingTick(double deltatime) override;

    PC_CORE_API RendererSystem(PC_CORE::RenderingWorldData* renderingWorldData);

    PC_CORE_API RendererSystem() = default;

    PC_CORE_API ~RendererSystem() override = default;
private:

    Signature m_StaticMeshSignature;

    Signature m_DirLightSignature;

    PC_CORE::RenderingWorldData* m_RenderingData;

    void ClearRenderingData();

    void PopulateStaticMeshes(const Level& _level);

    void PopulateLight(const Level& _level);

    
    REFLECT(RendererSystem)
    REFLECT_MEMBER(RendererSystem, m_StaticMeshSignature);
    REFLECT_MEMBER(RendererSystem, m_DirLightSignature);
};

END_PCCORE