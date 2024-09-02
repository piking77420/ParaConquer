#pragma once


#include "math/toolbox_typedef.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

struct DirLight
{
    //MAKE_COMPONENT(DirLight)
    Tbx::Vector3f color =  {1.f,1.f,1.f};
    float intensity = 1.f;
};
//REFLECT(DirLight,color)
//REFLECT(DirLight,intensity)

struct PointLight
{
    //MAKE_COMPONENT(PointLight)
    Tbx::Vector3f color =  {1.f,1.f,1.f};
    float intensity = 1.f;
};
//REFLECT(PointLight,color)
//REFLECT(PointLight,intensity)

struct SpotLight
{
    //MAKE_COMPONENT(SpotLight)
    Tbx::Vector3f color = {1.f,1.f,1.f};
    float intensity = 1.f;
};
//REFLECT(SpotLight,color)
//REFLECT(SpotLight,intensity)

END_PCCORE