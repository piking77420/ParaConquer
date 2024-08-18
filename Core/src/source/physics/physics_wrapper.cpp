#include "physics/physics_wrapper.hpp"

#include "app.hpp"
#include "physics/box_collider.hpp"
#include "physics/rigid_body.hpp"
#include "physics/sphere_collider.hpp"
#include "time/core_time.hpp"
#include "time/timer.hpp"
#include "world/transform.hpp"

using namespace PC_CORE; 

void PhysicsWrapper::InitBodies(Scene* _scene)
{
    InitSphereCollider(_scene);
    InitBoxCollider(_scene);
    std::vector<BoxCollider>* sphereColliders = nullptr;
    _scene->GetComponentData<BoxCollider>(&sphereColliders);
    const Tbx::Vector3f force = {-1000,1000,1000};
    AddForce(sphereColliders->at(0).body_Id, {1000,1,1000});
    AddTorque(sphereColliders->at(0).body_Id, force);
}

void PhysicsWrapper::UpdatePhysics(float _deltatime, Scene* _scene)
{
    Timer::StartTimer("PhyscisPass");
    physicsEngine.Update(_deltatime);
    AppliePhyscisToTransform(_scene);
    Timer::EndTimer();
}

void PhysicsWrapper::AddForce(uint32_t _id, const Tbx::Vector3f& _force)
{
    PhysicsWrapperInstance->physicsEngine.AddForce(_id, _force);
}

void PhysicsWrapper::AddImpulse(uint32_t _id, const Tbx::Vector3f& _impulse)
{
    PhysicsWrapperInstance->physicsEngine.AddImpulse(_id, _impulse);
}

void PhysicsWrapper::AddTorque(uint32_t _id, const Tbx::Vector3f& _Torque)
{
    PhysicsWrapperInstance->physicsEngine.AddTorque(_id, _Torque);
}

void PhysicsWrapper::CreateMeshInfo(void* _verticiesData, uint32_t _nbrOfVerticies, uint32_t _vertexSize,
    uint32_t _vectorSize, uint32_t _vectorDataSize, uint32_t _positionOffSet)
{
    PhysicsWrapperInstance->physicsEngine.CreateMeshInfo(_verticiesData, _nbrOfVerticies,_vertexSize,_vectorSize,_vectorDataSize, _positionOffSet);
}

Tbx::Vector3<MotionCore::numeric> PhysicsWrapper::GetPosition(uint32_t _id)
{
    return PhysicsWrapperInstance->physicsEngine.GetPosition(_id);
}

Tbx::Quaternion<MotionCore::numeric> PhysicsWrapper::GetRotation(uint32_t _id)
{
    return PhysicsWrapperInstance->physicsEngine.GetRotation(_id);
}

PhysicsWrapper::PhysicsWrapper()
{
    PhysicsWrapperInstance = this;
    const MotionCore::PhyscicsEngineInitilizationParameters initilizationParameters =
        {
        .timeStep = timeStep
        };
    physicsEngine.Initialize(&initilizationParameters);
}

PhysicsWrapper::~PhysicsWrapper()
{
    physicsEngine.Destroy();
}

void PhysicsWrapper::InitSphereCollider(Scene* scene)
{
    std::vector<SphereCollider>* sphereColliders = nullptr;
    scene->GetComponentData<SphereCollider>(&sphereColliders);

    for (SphereCollider& s : *sphereColliders)
    {
        if (s.componentHolder.entityID == NULL_ENTITY)
            continue;
        
        Transform* transform = scene->GetComponent<Transform>(s.componentHolder.entityID);
        if (transform == nullptr)
        {
            transform = scene->AddComponent<Transform>(s.componentHolder.entityID);
        }

        RigidBody* rigidBody = scene->GetComponent<RigidBody>(s.componentHolder.entityID);
        if (rigidBody == nullptr)
        {
            rigidBody = scene->AddComponent<RigidBody>(s.componentHolder.entityID);
        }
        
        MotionCore::BodyCreateInfo _bodyCreateInfo =
            {
            .position = transform->position,
            .mass = rigidBody->mass,
            .rotation = transform->rotation,
            .physcicalMaterial =
                {
                .damping = rigidBody->damping,
                .angularDamping = rigidBody->angularDamping,
                .restitutionCoeff = rigidBody->restitutionCoef
                }
            };

        _bodyCreateInfo.bodyTypeInfo.position = transform->position;
        _bodyCreateInfo.bodyTypeInfo.data.sphere.radius = s.radius;
        _bodyCreateInfo.bodyTypeInfo.bodyType = MotionCore::SPHERE;
        
        s.body_Id = physicsEngine.CreateBody(_bodyCreateInfo);
    }

}

void PhysicsWrapper::InitBoxCollider(Scene* _scene)
{
    std::vector<BoxCollider>* boxCollider = nullptr;
    _scene->GetComponentData<BoxCollider>(&boxCollider);

    for (BoxCollider& box : *boxCollider)
    {
        if (box.componentHolder.entityID == NULL_ENTITY)
            continue;
        
        Transform* transform = _scene->GetComponent<Transform>(box.componentHolder.entityID);
        if (transform == nullptr)
        {
            transform = _scene->AddComponent<Transform>(box.componentHolder.entityID);
        }

        RigidBody* rigidBody = _scene->GetComponent<RigidBody>(box.componentHolder.entityID);
        if (rigidBody == nullptr)
        {
            rigidBody = _scene->AddComponent<RigidBody>(box.componentHolder.entityID);
        }
        
        MotionCore::BodyCreateInfo _bodyCreateInfo =
            {
            .position = transform->position + box.center,
            .mass = rigidBody->mass,
            .rotation = transform->rotation
            ,
            .physcicalMaterial =
                {
                .damping = rigidBody->damping,
                .angularDamping = rigidBody->angularDamping,
                .restitutionCoeff = rigidBody->restitutionCoef
                }
            };

        _bodyCreateInfo.bodyTypeInfo.position = box.center;
        _bodyCreateInfo.bodyTypeInfo.data.aabb.extend = box.size * 0.5f;
        _bodyCreateInfo.bodyTypeInfo.bodyType = MotionCore::BOX;
        
        box.body_Id = physicsEngine.CreateBody(_bodyCreateInfo);
    }
}

void PhysicsWrapper::DestroyBodies(Scene* _scene)
{
    std::vector<SphereCollider>* sphereColliders = nullptr;
    _scene->GetComponentData<SphereCollider>(&sphereColliders);
    std::vector<BoxCollider>* boxCollider = nullptr;
    _scene->GetComponentData<BoxCollider>(&boxCollider);

    for (const SphereCollider& s : *sphereColliders)
    {
        if (s.componentHolder.entityID == NULL_ENTITY)
            continue;
        
        physicsEngine.DestroyBody(s.body_Id);
    }

    for (const BoxCollider& box : *boxCollider)
    {
        if (box.componentHolder.entityID == NULL_ENTITY)
            continue;
        
        physicsEngine.DestroyBody(box.body_Id);
    }
    
}

void PhysicsWrapper::AppliePhyscisToTransform(Scene* _scene)
{
    std::vector<SphereCollider>* sphereColliders = nullptr;
    _scene->GetComponentData<SphereCollider>(&sphereColliders);
    for (const SphereCollider& s : *sphereColliders)
    {
        if (s.componentHolder.entityID == NULL_ENTITY)
            continue;

        if (s.body_Id == MotionCore::NULLBODY)
            continue;
        
        Transform* transform = _scene->GetComponent<Transform>(s.componentHolder.entityID);

        const Tbx::Vector3f newPos = GetPosition(s.body_Id);
        const Tbx::Quaternionf newRot = GetRotation(s.body_Id);
        
        transform->position = newPos;
        transform->rotation = newRot;

    }

    std::vector<BoxCollider>* boxCollider = nullptr;
    _scene->GetComponentData<BoxCollider>(&boxCollider);

    for (const BoxCollider& box : *boxCollider)
    {
        if (box.componentHolder.entityID == NULL_ENTITY)
            continue;

        if (box.body_Id == MotionCore::NULLBODY)
            continue;
        
        Transform* transform = _scene->GetComponent<Transform>(box.componentHolder.entityID);

        const Tbx::Vector3f newPos = GetPosition(box.body_Id);
        const Tbx::Quaternionf newRot = GetRotation(box.body_Id);
        
        transform->position = newPos;
        transform->rotation = newRot;

    }

}

