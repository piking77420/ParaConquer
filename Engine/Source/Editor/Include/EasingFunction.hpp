#pragma once

#include "EditorHeader.hpp"
#include <Math/ToolBoxTypeDef.hpp>

BEGIN_EDITOR_PCCORE
    template <typename T>
    Tbx::Vector2<T> Lerp(Tbx::Vector2<T> a, Tbx::Vector2<T> b, float t)
    {
        return a + (b - a) * t;
    }

    template <typename T>
    Tbx::Vector2<T> SmoothDamp(const Tbx::Vector2<T>& current, const Tbx::Vector2<T>& target,
                               Tbx::Vector2<T>& currentVelocity, float smoothTime, float deltaTime)
    {
        float omega = 2.0f / smoothTime;
        float x = omega * deltaTime;
        float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

        Tbx::Vector2<T> change = current - target;
        Tbx::Vector2<T> temp = (currentVelocity + change * omega) * deltaTime;
        currentVelocity = (currentVelocity - temp * omega) * exp;
        return target + (change + temp) * exp;
    }

    template <typename T>
    Tbx::Vector3<T> Lerp(Tbx::Vector3<T> a, Tbx::Vector3<T> b, float t)
    {
        return a + (b - a) * t;
    }

    template <typename T>
    Tbx::Vector3<T> SmoothDamp(const Tbx::Vector3<T>& current, const Tbx::Vector3<T>& target,
                               Tbx::Vector3<T>& currentVelocity, float smoothTime, float deltaTime)
    {
        float omega = 2.0f / smoothTime;
        float x = omega * deltaTime;
        float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

        Tbx::Vector3<T> change = current - target;
        Tbx::Vector3<T> temp = (currentVelocity + change * omega) * deltaTime;
        currentVelocity = (currentVelocity - temp * omega) * exp;
        return target + (change + temp) * exp;
    }


END_EDITOR_PCCORE
