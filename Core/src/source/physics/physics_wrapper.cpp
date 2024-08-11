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
    float accumulateTime = _deltatime;
    
    while (accumulateTime >= timeStep)
    {
        physicsEngine.Update(timeStep);
        accumulateTime -= timeStep;
    }
}

PhysicsWrapper::PhysicsWrapper()
{
    physicsEngine.Initialize({timeStep});
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
