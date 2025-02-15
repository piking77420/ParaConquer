#pragma once


#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "ecs/ecs_front.h"

BEGIN_PCCORE

struct Light : public Component
{
  
};

REFLECT(Light,Component)

struct DirLight : public Light
{
    Tbx::Vector3f color =  {1.f,1.f,1.f};
    float intensity = 1.f;
    Tbx::Vector3f ambiant = {0.1f,0.1f,0.1f} ;
    bool isDirty = false;
};
REFLECT(DirLight,Light)
REFLECT_MEMBER(DirLight, color, MemberEnumFlag::COLOR)
REFLECT_MEMBER(DirLight, intensity)
REFLECT_MEMBER(DirLight, ambiant, MemberEnumFlag::COLOR)
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