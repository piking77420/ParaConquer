#pragma once

#include "RenderingTypedef.h"
#include "Ecs/EcsSystem.h"
#include "Resources/StaticMesh.hpp"
#include "Rendering/Material.hpp"
#include <DebugHelper/DebugDrawContext.hpp>


namespace PC_CORE
{
    class Level;
    class World;
}

namespace PC_CORE::Rendering
{
    struct StaticMeshComponentData
    {
        std::vector<const Material*> Materials; // make an array with MAX
        const StaticMesh* StaticMesh;
        Tbx::Matrix4x4d WorldMatrix;
    };

    struct DirLightData
    {
        Tbx::Vector3f LightColor;
        float LightIntensity;
        Tbx::Vector3f LightDirW;
    };

    struct LightData
    {
        enum LightType
        {
            PointLight,
            SpotLight
        };
        LightType LightType;
        Tbx::Vector3f LightColor; //  can be pack to 4 bit
        float LightIntensity;
        Tbx::Vector3d LightPosition;
        float Radius;
        Tbx::Vector3f LightDirection;
        float OuterAngle;
        float InnerAngle;
    };

    struct CaptureEnvironement
    {
        RhiTexture* Environement{ nullptr };
        RhiTexture* SkyBox{ nullptr };
        bool isDirty = false;
    };

    struct RenderingWorldData
    {

        DEFAULT_CONSTRUCTOR_DESTRUCTOR(RenderingWorldData);

        DEFAULT_COPY_MOVE_OPERATIONS(RenderingWorldData);

        void Clear()
        {
            StaticMeshComponentData.clear();
            LightsData.clear();
            DirLightData.reset();
            CaptureEnvironement.reset();
        }

        std::vector<Rendering::StaticMeshComponentData> StaticMeshComponentData;
        std::vector<LightData> LightsData;
        std::optional<DirLightData> DirLightData;

        // Debug
        std::array<std::vector<DebugDrawContext::DrawPrimitive>, static_cast<size_t>(DebugDrawContext::PrimitiveType::Count)> DebugDrawPrimitives;
        std::vector<DebugDrawContext::Frustum> DebugFrustums;
        std::optional<CaptureEnvironement> CaptureEnvironement;
        std::optional<RhiDescriptorSet*> SkyBox;
    };

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

        DebugDrawContext debugDrawContext; // use if def editor

    private:
        Rendering::RenderingWorldData m_GameRenderingWorldData;

        Signature m_StaticMeshSignature;

        Signature m_DirLightSignature;

        Signature m_PointLightSignature;

        Signature m_SpothLightSignature;

        void PopulateStaticMeshes(const Level& _level);

        void PopulateLight(const Level& _level);

        void PopulateDebugDraws();

        void PopulateEnvironementLighting(PC_CORE::World& World);

        REFLECT(RendererSystem)
        REFLECT_MEMBER(RendererSystem, m_StaticMeshSignature);
        REFLECT_MEMBER(RendererSystem, m_DirLightSignature);
    };

}
