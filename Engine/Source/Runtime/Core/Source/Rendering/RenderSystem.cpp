#include "Rendering/RenderSystem.hpp"

#include "Rendering/Light.hpp"
#include "World/Level.hpp"
#include "World/StaticMeshComponent.hpp"
#include "World/Transform.hpp"
#include "World/World.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Material.hpp"

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
        const StaticMeshComponent& staticMeshComponent = _level.GetComponent<StaticMeshComponent>(ent);
        const Transform& transform = _level.GetComponent<Transform>(ent);

        ObjectPtr<StaticMesh> mesh = staticMeshComponent.staticMesh.lock();
        std::vector<const Rendering::Material*> material;

        {
            if (staticMeshComponent.materials.empty())
            {
                if (mesh)
                {
                    auto& baseMaterialsWeak = mesh->GetBaseMaterial();
                    material.reserve(baseMaterialsWeak.size());
                    for (auto& it : baseMaterialsWeak)
                    {
                        if (const auto& locked = it.Lock())
                        {
                            material.emplace_back(locked.get());
                        }
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < staticMeshComponent.materials.size(); i++)
                {
                    material.push_back(staticMeshComponent.materials[i].Lock().Get());
                }
            }
 
        }
        
        if (!material.empty() && mesh)
        {
            const Tbx::Matrix4x4d m = Tbx::Trs4x4<double>(transform.Position,
                static_cast<Tbx::Quaterniond>(transform.Rotation.Quaternion),
                transform.Scale);

            const Rendering::StaticMeshComponentData staticMeshData =
            {
                .MaterialType = {},
                .Materials = std::move(material),
                .StaticMesh = mesh.get(),
                .WorldMatrix = m,
            };

            m_GameRenderingWorldData.StaticMeshComponentData.push_back(staticMeshData);
        }

        
    }
}

void PC_CORE::RendererSystem::PopulateLight(const Level& _level)
{
    PERF_REGION_SCOPED
    PERF_REGION_COLOR(PerfRegion::Game);

    std::set<EntityId>& staticMeshes = *GetEntitySet(m_DirLightSignature);
    for (auto& ent : staticMeshes)
    {
        const DirLight& Dir = _level.GetComponent<DirLight>(ent);
        const Transform& transform = _level.GetComponent<Transform>(ent);

        constexpr Tbx::Vector3f WorldUp = Tbx::Vector3f(0.0f, 1.0f, 0.0f);
        const Tbx::Matrix3x3f rot = Tbx::Rotation3x3<float>(transform.Rotation.Quaternion);
        const Tbx::Vector3f WorldUpRot = rot * WorldUp;

        m_GameRenderingWorldData.DirLightData.emplace(PC_CORE::Rendering::DirLightData
            {
                .LightDirW = WorldUpRot,
                .LightColor = Dir.color,
                .LightIntensity = Dir.intensity
            });
    }
}
