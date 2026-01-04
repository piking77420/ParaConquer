#pragma once

#include "RenderingTypedef.h"
#include "Ecs/EcsSystem.h"
namespace PC_CORE::Rendering
{
    struct RenderingWorldData;
}

BEGIN_PCCORE
    class Level;

    class RendererSystem : public EcsSystem
    {
    public:
        PC_CORE_API RendererSystem();

        PC_CORE_API ~RendererSystem() override = default;

        DEFAULT_COPY_MOVE_OPERATIONS(RendererSystem)

        IMP_DYNAMIC_REFLECT();

        PC_CORE_API void Begin() override
        {
        };

        PC_CORE_API void Tick(double deltaTime) override
        {
        };

        PC_CORE_API void RenderingTick(double deltatime) override;

        PC_CORE_API const Rendering::RenderingWorldData& GetRenderRenderingWorldData() const;

    private:
        Rendering::RenderingWorldData m_GameRenderingWorldData;

        Rendering::RenderingWorldData m_RenderRenderingWorldData;

        Signature m_StaticMeshSignature;

        Signature m_DirLightSignature;

        Signature m_PointLightSignature;

        void PopulateStaticMeshes(const Level& _level);

        void PopulateLight(const Level& _level);


        REFLECT(RendererSystem)
        REFLECT_MEMBER(RendererSystem, m_StaticMeshSignature);
        REFLECT_MEMBER(RendererSystem, m_DirLightSignature);
    };

END_PCCORE
