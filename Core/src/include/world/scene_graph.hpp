#pragma once

#include "scene.hpp"
#include "transform.hpp"
#include "math/toolbox_typedef.hpp"
#include "rendering/gpu_typedef.hpp"

BEGIN_PCCORE
class SceneGraph
{
public:
    std::vector<MatrixMeshes> globalMatricies;
    
    SceneGraph();

    ~SceneGraph() = default;
    
    PC_CORE_API void UpdateTransforms(Scene* _scene);
    
    PC_CORE_API void UpdateMatrix(Scene* _scene);

    PC_CORE_API static size_t MatrixMeshesSize();
    
    PC_CORE_API const Transform* GetParent(const Transform* transform);

    PC_CORE_API static inline bool HasParent(const Transform& transform);

private:
};

END_PCCORE