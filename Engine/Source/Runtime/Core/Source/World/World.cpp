#include "world/world.hpp"

#include "Rendering/Light.hpp"
#include "Rendering/RenderingTypedef.h"
#include "World/Transform.hpp"
#include "Rendering/RenderSystem.hpp"

using namespace PC_CORE;

World::World()
{
    if (m_World != nullptr)
    {
        PC_LOGERROR("World already initialized");
        throw std::runtime_error("World already initialized");
    }

    m_World = this;
    Rendering::RendererSystem* RenderSystem = World::GetWorld()->level.RegisterSystem<Rendering::RendererSystem>().get();
    m_DebugDrawContext = &RenderSystem->debugDrawContext;
}


void World::Begin()
{
    if (begin)
    {
        PERF_REGION_SCOPED;
        begin = false;
        level.Begin();
        run = true;
    }
}

void World::Update(double _tick)
{
    PERF_REGION_SCOPED;

    if (run)
    {
        level.Update(_tick);
    }
}

void World::RenderingTick(double _tick)
{
    level.RenderingTick(_tick);
}


void World::LoadLevel(const Level& _level)
{
    //level = _level;
    assert(false);
}

void World::DrawRay(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _dir, float _distance,
    Tbx::Vector3f _color)
{
    if (!m_DebugDrawContext)
        return;

    m_DebugDrawContext->PushRay(_p1, _dir, _distance, _color);
}

void World::DrawSphere(const Tbx::Vector3d& _p1, float _radius,
    Tbx::Vector3f _color)
{
    if (!m_DebugDrawContext)
        return;

    m_DebugDrawContext->PushSphereGizmo(DebugDrawContext::PrimitiveType::Sphere, _p1, _radius, _color);

}

void World::DrawBox(const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, const Tbx::Vector3d& _size,
    Tbx::Vector3f _color)
{
    if (!m_DebugDrawContext)
        return;

    m_DebugDrawContext->PushBoxGizmo(DebugDrawContext::PrimitiveType::Box, _p1, euler, _size, _color);
}

void World::DrawWireSphere(const Tbx::Vector3d& _p1, float _radius,
    Tbx::Vector3f _color)
{
    if (!m_DebugDrawContext)
        return;

    m_DebugDrawContext->PushSphereGizmo(DebugDrawContext::PrimitiveType::WireSphere, _p1, _radius, _color);
}

void World::DrawWireBox(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _euler, const Tbx::Vector3d& _size,
    Tbx::Vector3f _color)
{
    if (!m_DebugDrawContext)
        return;

    m_DebugDrawContext->PushBoxGizmo(DebugDrawContext::PrimitiveType::WireBox, _p1, _euler, _size, _color);
}

void World::DrawCapsule(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _euler, float _radius, float _height,
    Tbx::Vector3f _color)
{
    if (!m_DebugDrawContext)
        return;

    //m_DebugDrawContext->PushCapsuleGizmo(DebugDrawContext::PrimitiveType::Capsule, _p1, _euler, _radius, _height, _color);

}

void World::DrawWireCapsule(const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, float _radius, float _height,
    Tbx::Vector3f _color)
{
    if (!m_DebugDrawContext)
        return;

    //m_DebugDrawContext->PushCapsuleGizmo(DebugDrawContext::PrimitiveType::WireCapsule, _p1, euler, _radius, _height, _color);

}

void World::DrawFrustum(const Tbx::Matrix4x4d& _FrustumToWorld, Tbx::Vector3f _Color)
{
    if (!m_DebugDrawContext)
        return;

    m_DebugDrawContext->PushFrustum(_FrustumToWorld, false, _Color);
}

void World::DrawWireFrustum(const Tbx::Matrix4x4d& _FrustumToWorld, Tbx::Vector3f _Color)
{
    if (!m_DebugDrawContext)
        return;

    m_DebugDrawContext->PushFrustum(_FrustumToWorld, true, _Color);
}
