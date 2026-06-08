#include "Rendering/RenderSystem.hpp"

#include "Rendering/Light.hpp"
#include "World/Level.hpp"
#include "World/StaticMeshComponent.hpp"
#include "World/Transform.hpp"
#include "World/World.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Material.hpp"
#include "LowRenderer/Rhi.hpp"
#include "LowRenderer/RhiBuffer.h"
#include <Resources/Texture2d.hpp>
#include "App.hpp"

namespace PC_CORE::Rendering
{

RendererSystem::RendererSystem()
{
    DYNAMIC_REFLECT_INIT

    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Game);

    Level& l = PC_CORE::World::GetWorld()->level;

    m_StaticMeshSignature.set(l.GetComponentTypeBit<Transform>(), true);
    m_StaticMeshSignature.set(l.GetComponentTypeBit<StaticMeshComponent>(), true);
    AddSignature(m_StaticMeshSignature);

    m_DirLightSignature.set(l.GetComponentTypeBit<Transform>(), true);
    m_DirLightSignature.set(l.GetComponentTypeBit<DirLight>(), true);
    AddSignature(m_DirLightSignature);

    m_PointLightSignature.set(l.GetComponentTypeBit<Transform>(), true);
    m_PointLightSignature.set(l.GetComponentTypeBit<PointLight>(), true);
    AddSignature(m_PointLightSignature);

    m_SpothLightSignature.set(l.GetComponentTypeBit<Transform>(), true);
    m_SpothLightSignature.set(l.GetComponentTypeBit<SpotLight>(), true);
    AddSignature(m_SpothLightSignature);
}

void RendererSystem::RenderingTick(double deltatime)
{
    PERF_REGION_SCOPED;
    PERF_REGION_COLOR(PerfRegion::Game);

    const Level& l = World::GetWorld()->level;

    m_GameRenderingWorldData.Clear();
    PopulateEnvironementLighting(*World::GetWorld());
    PopulateStaticMeshes(l);
    PopulateLight(l);
    PopulateDebugDraws();
    debugDrawContext.ClearForNextFrame();
}

const Rendering::RenderingWorldData& RendererSystem::GetRenderRenderingWorldData() const
{
    return m_GameRenderingWorldData;
}

void RendererSystem::PopulateStaticMeshes(const Level& _level)
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

            const Rendering::StaticMeshComponentData StaticMeshComponentData = m_GameRenderingWorldData.StaticMeshComponentData.emplace_back(
                std::move(material),
                mesh.get(),
                m
            );
      
#ifdef WITH_EDITOR
            if (staticMeshComponent.ShowBound)
            {
                MotionCore::Aabb<double> aabbx = mesh->GetAabb();
                aabbx = aabbx.GetTransformed(m);
                debugDrawContext.PushBoxGizmo(DebugDrawContext::PrimitiveType::WireBox, aabbx.GetCenter(), Tbx::Vector3d(0.0, 0.0, 0.0), aabbx.GetSize(), Tbx::Vector3f(0.f, 1.0f, 0.f));
            }

            if (staticMeshComponent.ShowDrawCommandsBounds)
            {
                auto& Data = mesh.get()->GetStaticMeshData();
                for (size_t i = 0; i < Data.MeshLods[0].DrawCommands.size(); i++)
                {
                    MotionCore::Aabb<double> aabbx = Data.MeshLods[0].DrawCommands[i].GlobalModelAABB;
                    aabbx = aabbx.GetTransformed(m);
                    debugDrawContext.PushBoxGizmo(DebugDrawContext::PrimitiveType::WireBox, aabbx.GetCenter(), Tbx::Vector3d(0.0, 0.0, 0.0), aabbx.GetSize(), Tbx::Vector3f(0.f, 1.0f, 0.f));
                }
            }
#endif
        }
    }
}

void RendererSystem::PopulateLight(const Level& _level)
{
    PERF_REGION_SCOPED
    PERF_REGION_COLOR(PerfRegion::Game);

    {
        std::set<EntityId>& Dirls = *GetEntitySet(m_DirLightSignature);
        for (auto& ent : Dirls)
        {
            const DirLight& Dir = _level.GetComponent<DirLight>(ent);
            const Transform& transform = _level.GetComponent<Transform>(ent);

            constexpr Tbx::Vector3f WorldUp = Tbx::Vector3f(0.0f, 1.0f, 0.0f);
            const Tbx::Matrix3x3f rot = Tbx::Rotation3x3<float>(transform.Rotation.Quaternion);
            const Tbx::Vector3f WorldUpRot = rot * WorldUp;

            m_GameRenderingWorldData.DirLightData.emplace(PC_CORE::Rendering::DirLightData
                {
                    .LightColor = Dir.color,
                    .LightIntensity = Dir.intensity,
                    .LightDirW = WorldUpRot
                });
        }
    }

    {
        std::set<EntityId>& pointLights = *GetEntitySet(m_PointLightSignature);
        for (auto& ent : pointLights)
        {
            const PointLight& Point = _level.GetComponent<PointLight>(ent);
            const Transform& transform = _level.GetComponent<Transform>(ent);

            m_GameRenderingWorldData.LightsData.emplace_back(
                Rendering::LightData::LightType::PointLight, // LightType
                Point.color, // LightColor
                Point.intensity, // LightIntensity
                transform.Position, // Radius
                Point.Radius, // Radius
                Tbx::Vector3f(), // LightDirection
                0.f, // OuterAngle
                0.f); //InnerAngle
        }
    }


    {
        std::set<EntityId>& SpothLights = *GetEntitySet(m_PointLightSignature);
        for (auto& ent : SpothLights)
        {
            const SpotLight& Spoth = _level.GetComponent<SpotLight>(ent);
            const Transform& transform = _level.GetComponent<Transform>(ent);

            constexpr Tbx::Vector3f WorldFoward = Tbx::Vector3f::UnitZ();
            const Tbx::Matrix3x3f rot = Tbx::Rotation3x3<float>(transform.Rotation.Quaternion);
            const Tbx::Vector3f WorldForward = rot * WorldFoward;

            m_GameRenderingWorldData.LightsData.emplace_back(
                Rendering::LightData::LightType::SpotLight, // LightType
                Spoth.color, // LightColor
                Spoth.intensity, // LightIntensity
                transform.Position, // Radius
                Spoth.Radius, // Radius
                WorldForward, // LightDirection
                Spoth.OuterAngle, // OuterAngle
                Spoth.InnerAngle); //InnerAngle
        }
    }
    
}

void RendererSystem::PopulateDebugDraws()
{
    PERF_REGION_SCOPED
    PERF_REGION_COLOR(PerfRegion::Game);

    m_GameRenderingWorldData.DebugDrawPrimitives = debugDrawContext.DebugDrawPrimitives();
    m_GameRenderingWorldData.DebugFrustums = debugDrawContext.DebugDrawFrustums();
}

void RendererSystem::PopulateEnvironementLighting(PC_CORE::World& World)
{
    PERF_REGION_SCOPED
    PERF_REGION_COLOR(PerfRegion::Game);

    std::visit(
        overloaded{
            [&](WORLD::Environement::ImageBaseLighting& ibl)
            {  
                if (auto lock = ibl.EnvironementTexture.Lock())
                {
                    if (ibl.Skybox)
                    {
                        auto& env = m_GameRenderingWorldData.CaptureEnvironement.emplace();
                        env = {
                            .Environement = lock.Get()->Get(),
                            .SkyBox = ibl.Skybox.get(),
                            .isDirty = ibl.isDiry,
                        };
                        //ibl.isDiry = false;
                    }
                }
            }
        }, World.Environement.GetEnvironementLighting());

}

}