#include "world/scene_graph.hpp"

#include "ecs/ecs_header.h"
#include "math/matrix_transformation.hpp"
#include "rendering/gpu_typedef.hpp"
#include "world/transform.hpp"

using namespace PC_CORE;

SceneGraph::SceneGraph()
{
    globalMatricies.resize(MAX_ENTITIES);
}


void SceneGraph::UpdateTransforms(Scene* _scene)
{
    /*
    Scene& scene = *_scene;
    scene.GetComponentData<Transform>(&m_transforms);

    for (int i = 0; i < m_transforms->size(); ++i)
    {
        Transform& transform = m_transforms->at(i);

        if (!IsValid(transform.componentHolder))
            continue;

        // TODO
        
        
    }

    */
}

void SceneGraph::UpdateMatrix(Scene* _scene)
{
     Scene& scene = *_scene;
    
    std::vector<Transform>& transforms = *scene.GetData<Transform>();

    for (Transform& transform : transforms)
    {
        MatrixMeshes& matricies = globalMatricies[transform.entityId];
        Trs3D(transform.position, transform.rotation.Normalize(), transform.scale, &matricies.model);
        Invert<float>(matricies.model, &matricies.modelNormalMatrix);
        matricies.modelNormalMatrix = matricies.modelNormalMatrix.Transpose();
    }
    
}

size_t SceneGraph::MatrixMeshesSize()
{
    return sizeof(MatrixMeshes) * MAX_ENTITIES;
}


const Transform* SceneGraph::GetParent(const Transform* transform)
{
    // TODO Update ECS
    /*
    
    for (int i = 0; i < m_transforms->size(); ++i)
    {
        Transform& transform = m_transforms->at(i);

        if (transform.componentHolder.entityID != transform.parentId)
            continue;
        
        if (!IsValid(transform.componentHolder))
        {
            PC_LOGERROR("This entity parent was found but the parent is invalid")
            return nullptr;
        }

        return &transform;
        
    }

    
    PC_LOGERROR("This entity parent is invalid")*/
    return nullptr;
}

bool SceneGraph::HasParent(const Transform& transform)
{
    return transform.parentId != INVALID_ENTITY_ID;
}
