#pragma once

#include "core_header.hpp"
#include "core/physics_engine.h"
#include "world/scene.hpp"

BEGIN_PCCORE
class PhysicsWrapper
{
public:
    static constexpr float timeStep = 0.001f;
    
    void InitBodies(Scene* _scene);

    void DestroyBodies(Scene* _scene);
    
    void UpdatePhysics(float _deltatime, Scene* _scene);
    
    static void AddForce(uint32_t _id, const Tbx::Vector3f& _force);

    static void AddImpulse(uint32_t _id, const Tbx::Vector3f& _impulse);

    static void AddTorque(uint32_t _id, const Tbx::Vector3f& _torque);

    static void AddImpulseRotation(uint32_t _id, Tbx::Vector3f _impulseRot);

    static void CreateMeshInfo(
        void* _verticiesData, uint32_t _nbrOfVerticies, uint32_t _vertexSize, uint32_t _vectorSize, uint32_t _vectorDataSize,  uint32_t _positionOffSet);

    static Tbx::Vector3<MotionCore::numeric> GetPosition(uint32_t _id);

    static Tbx::Quaternion<MotionCore::numeric> GetRotation(uint32_t _id);
    
    PhysicsWrapper();

    ~PhysicsWrapper();
private:

    static inline PhysicsWrapper* PhysicsWrapperInstance = nullptr;
    
    void InitSphereCollider(Scene* _scene);

    void InitBoxCollider(Scene* _scene);
    
    void AppliePhyscisToTransform(Scene* _scene);
    
    MotionCore::PhysicsEngine physicsEngine;
};

END_PCCORE