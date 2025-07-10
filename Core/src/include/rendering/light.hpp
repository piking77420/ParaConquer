#pragma once


#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "ecs/ecs_front.h"

BEGIN_PCCORE

struct Light : public Component
{
    bool isDirty = false;
};

REFLECT(Light,Component)

struct DirLight : public Light
{
    Tbx::Vector3f color = Tbx::Vector3f(1.f,1.f,1.f);
    float intensity = 1.f;
    Tbx::Vector3f ambiant = Tbx::Vector3f(0.1f,0.1f,0.1f);
};
REFLECT(DirLight, Light)
REFLECT_MEMBER(DirLight, color, COLOR)
REFLECT_MEMBER(DirLight, intensity)
REFLECT_MEMBER(DirLight, ambiant, COLOR)
REFLECT_MEMBER(DirLight, isDirty)


struct PointLight : public Light
{
  
};
REFLECT(PointLight, Light)


struct SpotLight : public Light
{
    float outerCutOff;
    float innerCutOff;
};
REFLECT(SpotLight, Light)
REFLECT_MEMBER(SpotLight, outerCutOff)
REFLECT_MEMBER(SpotLight, innerCutOff)


END_PCCORE