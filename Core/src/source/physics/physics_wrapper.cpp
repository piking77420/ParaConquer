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
    std::vector<BoxCollider>* sphereColliders = _scene->GetData<BoxCollider>();
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

void PhysicsWrapper::InitSphereCollider(Scene* _scene)
{

    std::vector<SphereCollider>* sphereColliders = _scene->GetData<SphereCollider>();

    for (SphereCollider& s : *sphereColliders)
    {

        if (s.entityId == INVALID_ENTITY_ID)
            continue;

        Transform* transform = _scene->GetComponent<Transform>(_scene->GetEntityFromId(s.entityId));
        if (transform == nullptr)
            return;

        RigidBody* rigidBody = _scene->GetComponent<RigidBody>(_scene->GetEntityFromId(s.entityId));
        if (rigidBody == nullptr)
            return;
        
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
        
        _bodyCreateInfo.bodyTypeInfo.data.sphere.radius = s.radius;
        _bodyCreateInfo.bodyTypeInfo.bodyType = MotionCore::SPHERE;
        
        s.body_Id = physicsEngine.CreateBody(_bodyCreateInfo);
    }

}

void PhysicsWrapper::InitBoxCollider(Scene* _scene)
{
    std::vector<BoxCollider>* boxCollider = _scene->GetData<BoxCollider>();

    for (BoxCollider& b : *boxCollider)
    {

        if (b.entityId == INVALID_ENTITY_ID)
            continue;

        Transform* transform = _scene->GetComponent<Transform>(_scene->GetEntityFromId(b.entityId));
        if (transform == nullptr)
            return;

        RigidBody* rigidBody = _scene->GetComponent<RigidBody>(_scene->GetEntityFromId(b.entityId));
        if (rigidBody == nullptr)
            return;

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

        _bodyCreateInfo.bodyTypeInfo.data.aabb.extend = b.extend;
        _bodyCreateInfo.bodyTypeInfo.bodyType = MotionCore::BOX;

        b.body_Id = physicsEngine.CreateBody(_bodyCreateInfo);
    }
}

void PhysicsWrapper::DestroyBodies(Scene* _scene)
{
    
    std::vector<SphereCollider>* sphereColliders = _scene->GetData<SphereCollider>();
    std::vector<BoxCollider>* boxCollider = _scene->GetData<BoxCollider>();

    for (const SphereCollider& s : *sphereColliders)
    {
        if (s.entityId == INVALID_ENTITY_ID)
            continue;
        
        physicsEngine.DestroyBody(s.body_Id);
    }

    for (const BoxCollider& box : *boxCollider)
    {
        if (box.entityId == INVALID_ENTITY_ID)
            continue;
        
        physicsEngine.DestroyBody(box.body_Id);
    }
    
}

void PhysicsWrapper::AppliePhyscisToTransform(Scene* _scene)
{

    std::vector<SphereCollider>* sphereColliders = _scene->GetData<SphereCollider>();
    std::vector<BoxCollider>* boxCollider = _scene->GetData<BoxCollider>();

    for (const SphereCollider& s : *sphereColliders)
    {
        if (s.entityId == INVALID_ENTITY_ID)
            continue;

        if (s.body_Id == MotionCore::NULLBODY)
            continue;
        
        Transform* transform = _scene->GetComponent<Transform>(_scene->GetEntityFromId(s.entityId));

        const Tbx::Vector3f newPos = GetPosition(s.body_Id);
        const Tbx::Quaternionf newRot = GetRotation(s.body_Id);
        
        transform->position = newPos;
        transform->rotation = newRot;

    }

    for (const BoxCollider& box : *boxCollider)
    {
        if (box.entityId == INVALID_ENTITY_ID)
            continue;

        if (box.body_Id == MotionCore::NULLBODY)
            continue;
        
        Transform* transform = _scene->GetComponent<Transform>(_scene->GetEntityFromId(box.entityId));

        const Tbx::Vector3f newPos = GetPosition(box.body_Id);
        const Tbx::Quaternionf newRot = GetRotation(box.body_Id);
        
        transform->position = newPos;
        transform->rotation = newRot;

    }
}

