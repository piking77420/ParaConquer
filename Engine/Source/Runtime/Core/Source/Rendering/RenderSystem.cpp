#include "Rendering/RenderSystem.hpp"

#include "Rendering/Light.hpp"
#include "World/Level.hpp"
#include "World/StaticMeshComponent.hpp"
#include "World/Transform.hpp"
#include "World/World.hpp"
#include "Rendering/Renderer.hpp"

PC_CORE::RendererSystem::RendererSystem()
{
    DYNAMIC_REFLECT_INIT

    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Game);

    Level& l = World::GetWorld()->level;

    m_StaticMeshSignature.set(l.GetComponentTypeBit<Transform>(), true);
    m_StaticMeshSignature.set(l.GetComponentTypeBit<StaticMeshComponent>(), true);
    AddSignature(m_StaticMeshSignature);

    m_DirLightSignature.set(l.GetComponentTypeBit<Transform>(), true);
    m_DirLightSignature.set(l.GetComponentTypeBit<DirLight>(), true);
    AddSignature(m_DirLightSignature);

    m_PointLightSignature.set(l.GetComponentTypeBit<Transform>(), true);
    m_PointLightSignature.set(l.GetComponentTypeBit<PointLight>(), true);
    AddSignature(m_PointLightSignature);
}

void PC_CORE::RendererSystem::RenderingTick(double deltatime)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Game);

    const Level& l = World::GetWorld()->level;

    m_GameRenderingWorldData.Clear();
    PopulateStaticMeshes(l);
    PopulateLight(l);

    // Make a copy
    m_RenderRenderingWorldData = m_GameRenderingWorldData;
}

const PC_CORE::Rendering::RenderingWorldData& PC_CORE::RendererSystem::GetRenderRenderingWorldData() const
{
    return m_RenderRenderingWorldData;
}

void PC_CORE::RendererSystem::PopulateStaticMeshes(const Level& _level)
{
    PERF_REGION_SCOPED
    PERF_REGION_COLOR(PerfRegion::Game);
    
    std::set<EntityId>& staticMeshes = *GetEntitySet(m_StaticMeshSignature);

    for (auto& ent : staticMeshes)
    {
        const StaticMeshComponent& staticMesh = _level.GetComponent<StaticMeshComponent>(ent);
        const Transform& transform = _level.GetComponent<Transform>(ent);

        std::shared_ptr<StaticMesh> mesh = staticMesh.staticMesh.lock();
        std::shared_ptr<Material> material = staticMesh.material.lock();

        if (!mesh /*|| !material*/)
            return;


        const Tbx::Matrix4x4d m = Tbx::Trs4x4<double>(transform.Position,
                                                      static_cast<Tbx::Quaterniond>(transform.Rotation.Quaternion),
                                                      transform.Scale);
        const Rendering::StaticMeshComponentData staticMeshData =
        {
            .MaterialType = {},
            .DescriptorSet = nullptr,
            .StaticMesh = mesh.get(),
            .WorldMatrix = m,
            .NormalInvertMatrix = m.Invert().Transpose(),
        };

        m_GameRenderingWorldData.StaticMeshComponentData.push_back(staticMeshData);
    }
}

void PC_CORE::RendererSystem::PopulateLight(const Level& _level)
{
    PERF_REGION_SCOPED
    PERF_REGION_COLOR(PerfRegion::Game);

    /*
    std::set<EntityId>& dirLights = *GetEntitySet(m_DirLightSignature);
    for (auto& ent : dirLights)
    {
        const DirLight& dirLight = _level.GetComponent<DirLight>(ent);
        const Transform& transform = _level.GetComponent<Transform>(ent);

        LightData lightData;
        lightData.LightType = LightType::Directional;
        lightData.Data.DirectionalLight =
        {
            .Color = dirLight.color,
            .Intensity = dirLight.intensity,
            .Direction = Tbx::Quaternionf::ToEulerAngles(transform.Rotation.Quaternion.Normalize())
        };

        m_RenderingDataPtr->LightData.push_back(lightData);
    }

    std::set<EntityId>& pointLights = *GetEntitySet(m_PointLightSignature);

    for (auto& ent : pointLights)
    {
        const PointLight& pointLight = _level.GetComponent<PointLight>(ent);
        const Transform& transform = _level.GetComponent<Transform>(ent);

        LightData lightData;
        lightData.LightType = LightType::Point;
        lightData.Data.PointLightData =
        {
            .Color = pointLight.color,
            .Intensity = pointLight.intensity,
            .Position = transform.Position,
        };

        m_RenderingDataPtr->LightData.push_back(lightData);
    }*/
}
