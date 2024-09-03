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
    
    void UpdateTransforms(Scene* _scene);
    
    void UpdateMatrix(Scene* _scene);

    static size_t MatrixMeshesSize();
    
    const Transform* GetParent(const Transform* transform);

    static inline bool HasParent(const Transform& transform);    

private:
};

END_PCCORE