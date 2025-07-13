#pragma once


#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "ecs/ecs_front.h"

BEGIN_PCCORE

struct Light : public Component
{
    bool isDirty = false;
    Tbx::Vector3f color = Tbx::Vector3f(1.f,1.f,1.f);
    float intensity = 1.f;
};

REFLECT(Light,Component)
REFLECT_MEMBER(Light, color, COLOR)
REFLECT_MEMBER(Light, intensity)

struct DirLight : public Light
{
   
};
REFLECT(DirLight, Light)



struct PointLight : public Light
{
};
REFLECT(PointLight, Light)
REFLECT_MEMBER(DirLight, isDirty)

struct SpotLight : public Light
{
    float outerCutOff;
    float innerCutOff;
};
REFLECT(SpotLight, Light)
REFLECT_MEMBER(SpotLight, outerCutOff)
REFLECT_MEMBER(SpotLight, innerCutOff)


END_PCCORE