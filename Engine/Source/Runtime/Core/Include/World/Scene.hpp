#pragma once

#include <array>
#include <memory>
#include <vector>
#include <cassert>

#include "CoreHeader.hpp"
#include "Log.hpp"
#include "Ecs/EntityManager.h"
#include "Reflection/Reflector.hpp"


BEGIN_PCCORE
    class Scene
    {
    public:
        PC_CORE_API void Begin();

        PC_CORE_API void Update();

        DEFAULT_COPY_MOVE_OPERATIONS(Scene)

        PC_CORE_API Scene() = default;

        PC_CORE_API ~Scene() = default;
    };


END_PCCORE
