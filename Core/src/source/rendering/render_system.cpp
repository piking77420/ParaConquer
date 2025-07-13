#include "rendering/render_system.hpp"

#include "rendering/light.hpp"
#include "world/level.hpp"
#include "world/static_mesh.hpp"
#include "world/transform.hpp"
#include "world/world.hpp"

PC_CORE::RendererSystem::RendererSystem(PC_CORE::RenderingWorldData* renderingWorldData) : m_RenderingData(renderingWorldData)
{
    DYNAMIC_REFLECT_INIT

    PERF_REGION_SCOPED;
    Level& l = World::GetWorld()->level;
    
    m_StaticMeshSignature.set(l.GetComponentTypeBit<Transform>(), true);
    m_StaticMeshSignature.set(l.GetComponentTypeBit<StaticMesh>(), true);
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

    ClearRenderingData();
    
    const Level& l = World::GetWorld()->level;

    PopulateStaticMeshes(l);
    PopulateLight(l);
}

void PC_CORE::RendererSystem::ClearRenderingData()
{
    assert(m_RenderingData != nullptr);
    m_RenderingData->lightData.clear();
    m_RenderingData->staticMeshData.clear();
}

void PC_CORE::RendererSystem::PopulateStaticMeshes(const Level& _level)
{
    std::set<EntityId>& staticMeshes = *GetEntitySet(m_StaticMeshSignature);

    for (auto& ent : staticMeshes)
    {
        const StaticMesh& staticMesh = _level.GetComponent<StaticMesh>(ent);
        const Transform& transform = _level.GetComponent<Transform>(ent);

        std::shared_ptr<Mesh> mesh = staticMesh.mesh.lock();
        std::shared_ptr<Material> material = staticMesh.material.lock();

        if (!mesh || !material)
            return;


        const Tbx::Matrix4x4d m = Tbx::Trs4x4<double>(transform.position, static_cast<Tbx::Quaterniond>(transform.rotation.quaternion),
            transform.scale);
        const StaticMeshData staticMeshData =
        {
        .materialType = material->materialType,
        .descriptorSet = material->GetDescriptorSet(),
        .mesh = mesh.get(),
        .worldMatrix = m ,
        .normalInvertMatrix = m.Invert().Transpose(),
        };

        m_RenderingData->staticMeshData.push_back(staticMeshData);
    }
        
   
}

void PC_CORE::RendererSystem::PopulateLight(const Level& _level)
{
    std::set<EntityId>& dirLights = *GetEntitySet(m_DirLightSignature);
    for (auto& ent : dirLights)
    {
        const DirLight& dirLight = _level.GetComponent<DirLight>(ent);
        const Transform& transform = _level.GetComponent<Transform>(ent);

        LightData lightData;
        lightData.lightType = LightType::Directional;
        lightData.data.directionalLight  =
        {
        .color = dirLight.color,
        .intensity = dirLight.intensity,
        .direction = Tbx::Quaternionf::ToEulerAngles(transform.rotation.quaternion.Normalize())
        };

        m_RenderingData->lightData.push_back(lightData);
    }

    std::set<EntityId>& pointLights = *GetEntitySet(m_PointLightSignature);

    for (auto& ent : pointLights)
    {
        const PointLight& pointLight = _level.GetComponent<PointLight>(ent);
        const Transform& transform = _level.GetComponent<Transform>(ent);

        LightData lightData;
        lightData.lightType = LightType::Point;
        lightData.data.pointLightData  =
        {
          .color = pointLight.color,
          .intensity = pointLight.intensity,
          .position = transform.position,
            };

        m_RenderingData->lightData.push_back(lightData);
    }
}

