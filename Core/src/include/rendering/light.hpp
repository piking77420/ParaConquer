#pragma once

#include "world/component.hpp"

#include "math/toolbox_typedef.hpp"

BEGIN_PCCORE

struct DirLight
{
    MAKE_COMPONENT(DirLight)
    Vector3f color =  {1.f,1.f,1.f};
    float intensity = 1.f;
};
struct PointLight
{
    MAKE_COMPONENT(PointLight)
    Vector3f color =  {1.f,1.f,1.f};
    float intensity = 1.f;
};
struct SpotLight
{
    MAKE_COMPONENT(SpotLight)
    Vector3f color = {1.f,1.f,1.f};
    float intensity = 1.f;
};

END_PCCORE