#pragma once


#include "Math/ToolboxTypedef.hpp"
#include "Reflection/Reflector.hpp"
#include "Ecs/EcsFront.h"

BEGIN_PCCORE

struct Light : public Component
{
    bool isDirty = true;
    Tbx::Vector3f color = Tbx::Vector3f(1.f,1.f,1.f);
    float intensity = 1.f;

    DEFAULT_CONSTRUCTOR_DESTRUCTOR(Light);
};

REFLECT(Light,Component)
REFLECT_MEMBER(Light, isDirty)
REFLECT_MEMBER(Light, color, COLOR)
REFLECT_MEMBER(Light, intensity)

struct DirLight : public Light
{
    DEFAULT_CONSTRUCTOR_DESTRUCTOR(DirLight);
};
REFLECT(DirLight, Light)

struct SpotLight : public Light
{
    float outerCutOff;
    float innerCutOff;
};
REFLECT(SpotLight, Light)
REFLECT_MEMBER(SpotLight, outerCutOff)
REFLECT_MEMBER(SpotLight, innerCutOff)



struct PointLight : public Light
{
    DEFAULT_CONSTRUCTOR_DESTRUCTOR(PointLight);

};
REFLECT(PointLight, Light)

END_PCCORE