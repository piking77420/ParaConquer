#pragma once

#include "RenderingTypedef.h"
#include "Ecs/EcsSystem.h"
#include "Resources/StaticMesh.hpp"
#include "Rendering/Material.hpp"


namespace PC_CORE
{
    class Level;
}

namespace PC_CORE::Rendering
{
    struct StaticMeshComponentData
    {
        std::vector<const Material*> Materials; // make an array with MAX
        const StaticMesh* StaticMesh;
        Tbx::Matrix4x4d WorldMatrix;
        bool UseMeshlet;
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

    struct RenderingWorldData
    {

        DEFAULT_CONSTRUCTOR_DESTRUCTOR(RenderingWorldData);

        DEFAULT_COPY_MOVE_OPERATIONS(RenderingWorldData);

        void Clear()
        {
            StaticMeshComponentData.clear();
            LightsData.clear();
            DirLightData.reset();
        }

        std::vector<Rendering::StaticMeshComponentData> StaticMeshComponentData;
        std::vector<LightData> LightsData;
        std::optional<DirLightData> DirLightData;
    };

    /*struct RenderingWorldResource
    {
        DEFAULT_CONSTRUCTOR_DESTRUCTOR(RenderingWorldResource);

        DEFAULT_COPY_MOVE_OPERATIONS(RenderingWorldResource);

        std::unique_ptr<RhiBuffer> LightBuffer;

        std::unique_ptr<RhiBuffer> LightBufferHeader;
    };*/




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

        /*PC_CORE_API const Rendering::RenderingWorldResource& GetRenderRenderingWorldResource() const
        {
            return m_RenderingWorldResource;
        }*/


    private:
        Rendering::RenderingWorldData m_GameRenderingWorldData;

        Rendering::RenderingWorldData m_RenderRenderingWorldData;

       // Rendering::RenderingWorldResource m_RenderingWorldResource;

        Signature m_StaticMeshSignature;

        Signature m_DirLightSignature;

        Signature m_PointLightSignature;

        Signature m_SpothLightSignature;


        void PopulateStaticMeshes(const Level& _level);

        void PopulateLight(const Level& _level);


        REFLECT(RendererSystem)
        REFLECT_MEMBER(RendererSystem, m_StaticMeshSignature);
        REFLECT_MEMBER(RendererSystem, m_DirLightSignature);
    };

}
