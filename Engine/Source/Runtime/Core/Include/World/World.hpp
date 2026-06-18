#pragma once

#include "Ecs/EntityManager.h"
#include "Ecs/ComponentsManager.hpp"
#include "Ecs/SystemManager.hpp"
#include "Level.hpp"
#include <World/Environement.hpp>

BEGIN_PCCORE

class DebugDrawContext;

    class World
    {
    public:

        PC_CORE_API World();

        PC_CORE_API ~World()
        {
            m_World = nullptr;
        };

        PC_CORE_API FORCEINLINE static World* GetWorld()
        {
            return m_World;
        }

        PC_CORE_API void Begin();

        PC_CORE_API void Update(double _tick);

        PC_CORE_API void RenderingTick(double _tick);

        PC_CORE_API void LoadLevel(const Level& _level);

        PC_CORE_API void DrawRay(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _dir, float _distance = 1.f,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        PC_CORE_API void DrawSphere(const Tbx::Vector3d& _p1, float _radius = 0.5f,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        PC_CORE_API void DrawBox(const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, const Tbx::Vector3d& _size,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        PC_CORE_API void DrawWireSphere(const Tbx::Vector3d& _p1, float _radius = 0.5f,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        PC_CORE_API void DrawWireBox(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _euler, const Tbx::Vector3d& _size,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        PC_CORE_API void DrawCapsule(const Tbx::Vector3d& _p1, const Tbx::Vector3d& _euler, float _radius, float _height,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        PC_CORE_API void DrawWireCapsule(const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, float _radius, float _height,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        PC_CORE_API void DrawFrustum(const Tbx::Matrix4x4d& _FrustumToWorld, Tbx::Vector3f _Color);

        PC_CORE_API void DrawWireFrustum(const Tbx::Matrix4x4d& _FrustumToWorld, Tbx::Vector3f _Color);

        bool begin = false;

        bool run = false;

        Level level;

        WORLD::Environement Environement;

    private:
        PC_CORE_API static inline World* m_World = nullptr;

        PC_CORE::DebugDrawContext* m_DebugDrawContext = nullptr;

        REFLECT(World)
        REFLECT_MEMBER(World, level)
    };


END_PCCORE
