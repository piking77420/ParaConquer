#pragma once


#include "Math/ToolboxTypedef.hpp"
#include "Reflection/Reflector.hpp"
#include "Ecs/EcsFront.h"

BEGIN_PCCORE
    struct Light : Component
    {
        bool isDirty = true;
        Tbx::Vector3f color = Tbx::Vector3f(1.f, 1.f, 1.f);
        float intensity = 1.f;

        DEFAULT_CONSTRUCTOR_DESTRUCTOR(Light);
    };

    REFLECT(Light, Component)
    REFLECT_MEMBER(Light, isDirty)
    REFLECT_MEMBER(Light, color, Members::EnumFlag::Color);
    REFLECT_MEMBER(Light, intensity)

    struct DirLight : Light
    {
        DEFAULT_CONSTRUCTOR_DESTRUCTOR(DirLight);
    };

    REFLECT(DirLight, Light)

    struct SpotLight : Light
    {
        float outerCutOff;
        float innerCutOff;
    };

    REFLECT(SpotLight, Light)
    REFLECT_MEMBER(SpotLight, outerCutOff)
    REFLECT_MEMBER(SpotLight, innerCutOff)


    struct PointLight : Light
    {
        DEFAULT_CONSTRUCTOR_DESTRUCTOR(PointLight);
    };

    REFLECT(PointLight, Light)

END_PCCORE
