#pragma once

#include "CoreHeader.hpp"
#include "Ecs/EcsFront.h"
#include "Math/Quaternion.hpp"
#include "Math/ToolboxTypedef.hpp"

BEGIN_PCCORE
    struct Rotation
    {
        Tbx::Vector3f EulerAngles;
        Tbx::Quaternionf Quaternion = Tbx::Quaternionf::Identity();

        Rotation() = default;

        explicit Rotation(Tbx::Quaternionf q)
        {
            auto Qn = Quaternion.Normalize();
            Quaternion = Qn;
            EulerAngles = Tbx::Quaternionf::ToEulerAngles(Quaternion);
        }

        explicit Rotation(const Tbx::Vector3f _eulerAngle) : EulerAngles(_eulerAngle),
                                                             Quaternion(
                                                                 Tbx::Quaternionf::FromEuler(_eulerAngle).Normalize())
        {
        }

        ~Rotation() = default;
    };

    REFLECT(Rotation)
    REFLECT_MEMBER(Rotation, EulerAngles)
    REFLECT_MEMBER(Rotation, Quaternion)

    // TODO be more cache friendly for rendering fetch data
    struct Transform : Component
    {
        EntityId ParentId = INVALID_ENTITY_ID;

        Tbx::Vector3d Position;

        Rotation Rotation;

        Tbx::Vector3d Scale = Tbx::Vector3d::UnitOne();

        Tbx::Vector3d LocalPosition;

        // TO HANDLE MATRIX TO AVOID RECALCULATION
    };

    // TODO PUT THE REFLECT IN THE CLASS 


    REFLECT(Transform, Component)
    REFLECT_MEMBER(Transform, Position)
    REFLECT_MEMBER(Transform, Rotation)
    REFLECT_MEMBER(Transform, Scale)

END_PCCORE
