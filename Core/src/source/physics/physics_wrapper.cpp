#include "physics/physics_wrapper.hpp"

#include "physics/sphere_collider.hpp"
#include "time/core_time.hpp"

using namespace PC_CORE; 

void PhysicsWrapper::InitPhysicBody(Scene* _scene)
{
    InitSphereCollider(_scene);
}

void PhysicsWrapper::UpdatePhysics(float _deltatime, Scene* _scene)
{
        physicsEngine.Update(timeStep);
}

void PhysicsWrapper::AddForce(uint32_t _id, Tbx::Vector3f force)
{
    
}

void PhysicsWrapper::AddImpulse(uint32_t _id, Tbx::Vector3f force)
{
}

PhysicsWrapper::PhysicsWrapper()
{
    MotionCore::PhyscicsEngineInitilizationParameters initilizationParameters =
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

}

