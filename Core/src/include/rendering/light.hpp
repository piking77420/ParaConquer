#pragma once


#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "ecs/ecs_front.h"

BEGIN_PCCORE

struct Light : public Component
{
    Tbx::Vector3f color =  {1.f,1.f,1.f};
    float intensity = 1.f;
    bool isDirty = false;
};

REFLECT(Light,Component)
REFLECT_MEMBER(Light, color, MemberEnumFlag::COLOR)
REFLECT_MEMBER(Light, intensity)
REFLECT_MEMBER(Light, isDirty)

struct DirLight : public Light
{
};
REFLECT(DirLight,Light)


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