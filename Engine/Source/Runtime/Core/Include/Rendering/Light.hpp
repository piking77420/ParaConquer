#pragma once


#include "Math/ToolboxTypedef.hpp"
#include "Reflection/Reflector.hpp"
#include "Ecs/EcsFront.h"

BEGIN_PCCORE
    struct Light : public Component
    {
        Tbx::Vector3f color = Tbx::Vector3f(1.f, 1.f, 1.f);
        float intensity = 1.f;

    };

    REFLECT(Light, Component)
    REFLECT_MEMBER(Light, color, Members::EnumFlag::Color);
    REFLECT_MEMBER(Light, intensity)

    struct DirLight : public Light
    {

    };

    REFLECT(DirLight, Light)

   struct WorldLight : public Light
   {
        float Radius;
   };
    REFLECT(WorldLight, Light)
    REFLECT_MEMBER(WorldLight, Radius)

    struct SpotLight : public WorldLight
    {
        float OuterAngle;
        float InnerAngle;
    };

    REFLECT(SpotLight, Light)
    REFLECT_MEMBER(SpotLight, OuterAngle)
    REFLECT_MEMBER(SpotLight, InnerAngle)


    struct PointLight : public  WorldLight
    {
    };

    REFLECT(PointLight, WorldLight)

END_PCCORE
