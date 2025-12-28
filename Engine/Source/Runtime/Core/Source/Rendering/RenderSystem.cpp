#include "Rendering/RenderSystem.hpp"

#include "Rendering/Light.hpp"
#include "World/Level.hpp"
#include "World/StaticMeshComponent.hpp"
#include "World/Transform.hpp"
#include "World/World.hpp"
#include "Rendering/Renderer.hpp"

PC_CORE::RendererSystem::RendererSystem(Rendering::RenderingWorldData* _renderingWorldData)
{
    DYNAMIC_REFLECT_INIT

    PERF_REGION_SCOPED;
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


    m_RenderingDataPtr = _renderingWorldData;
    assert(m_RenderingDataPtr != nullptr);
}

void PC_CORE::RendererSystem::RenderingTick(double deltatime)
{
    PERF_REGION_SCOPED;

    const Level& l = World::GetWorld()->level;

    m_RenderingDataPtr->Clear();
    PopulateStaticMeshes(l);
    PopulateLight(l);
}

void PC_CORE::RendererSystem::PopulateStaticMeshes(const Level& _level)
{
    /*
    std::set<EntityId>& staticMeshes = *GetEntitySet(m_StaticMeshSignature);

    for (auto& ent : staticMeshes)
    {
        const StaticMeshComponent& staticMesh = _level.GetComponent<StaticMeshComponent>(ent);
        const Transform& transform = _level.GetComponent<Transform>(ent);

        std::shared_ptr<StaticMesh> mesh = staticMesh.staticMesh.lock();
        std::shared_ptr<Material> material = staticMesh.material.lock();

        if (!mesh || !material)
            return;


        const Tbx::Matrix4x4d m = Tbx::Trs4x4<double>(transform.Position,
                                                      static_cast<Tbx::Quaterniond>(transform.Rotation.Quaternion),
                                                      transform.Scale);
        const StaticMeshComponentData staticMeshData =
        {
            .MaterialType = material->MaterialType,
            .DescriptorSet = material->GetDescriptorSet(),
            .StaticMesh = mesh.get(),
            .WorldMatrix = m,
            .NormalInvertMatrix = m.Invert().Transpose(),
        };

        m_RenderingDataPtr->StaticMeshComponentData.push_back(staticMeshData);
    }*/
}

void PC_CORE::RendererSystem::PopulateLight(const Level& _level)
{
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
