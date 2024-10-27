#pragma once


#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"
#include "ecs/ecs_front.h"

BEGIN_PCCORE

struct DirLight : public Component
{
    Tbx::Vector3f color =  {1.f,1.f,1.f};
    float intensity = 1.f;
};
REFLECT(DirLight,Component)
REFLECT_MEMBER(DirLight, color, MemberEnumFlag::COLOR)
REFLECT_MEMBER(DirLight, intensity)

struct PointLight : public Component
{
    Tbx::Vector3f color =  {1.f,1.f,1.f};
    float intensity = 1.f;
};
REFLECT(PointLight, Component)
REFLECT_MEMBER(PointLight, color)
REFLECT_MEMBER(PointLight, intensity)

struct SpotLight : public Component
{
    Tbx::Vector3f color = {1.f,1.f,1.f};
    float intensity = 1.f;
};
REFLECT(SpotLight, Component)
REFLECT_MEMBER(SpotLight, color)
REFLECT_MEMBER(SpotLight, intensity)

END_PCCORE