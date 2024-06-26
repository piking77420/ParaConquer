#pragma once

#include "world/component.hpp"

#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

struct DirLight
{
    MAKE_COMPONENT(DirLight)
    Vector3f color =  {1.f,1.f,1.f};
    float intensity = 1.f;
};
REFLECT(DirLight,color)
REFLECT(DirLight,intensity)

struct PointLight
{
    MAKE_COMPONENT(PointLight)
    Vector3f color =  {1.f,1.f,1.f};
    float intensity = 1.f;
};
REFLECT(PointLight,color)
REFLECT(PointLight,intensity)

struct SpotLight
{
    MAKE_COMPONENT(SpotLight)
    Vector3f color = {1.f,1.f,1.f};
    float intensity = 1.f;
};
REFLECT(SpotLight,color)
REFLECT(SpotLight,intensity)

END_PCCORE