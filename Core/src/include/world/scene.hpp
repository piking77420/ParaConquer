#pragma once

#include <array>
#include <memory>
#include <vector>
#include <cassert>

#include "core_header.hpp"
#include "log.hpp"
#include "system.hpp"
#include "ecs/entity_register.h"


BEGIN_PCCORE
    class Scene
    {
    public:
        void Begin();
        
        void Update();

        Scene() = default;
        
        ~Scene() = default;
        
    private:
        
    };
END_PCCORE
