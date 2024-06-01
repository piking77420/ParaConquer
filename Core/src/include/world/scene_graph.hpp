#pragma once

#include "scene.hpp"
#include "world_header_typedef.hpp"
#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE
    class SceneGraph
{
public:
    SceneGraph();

    ~SceneGraph();
    
    void UpdateTransform(Scene* _scene);

    void UpdateMatrix(Scene* _scene);
    
private:
    std::vector<Matrix4x4f> matrixBuffer;    
};

END_PCCORE