#pragma once

#include <array>
#include <memory>
#include <vector>
#include <cassert>

#include "core_header.hpp"
#include "log.hpp"
#include "system.hpp"
#include "ecs/entity_register.h"
#include "reflection/reflector.hpp"


BEGIN_PCCORE
 
 
    
    class Scene
    {
    public:
        PC_CORE_API void Begin();
        
        PC_CORE_API void Update();

        DEFAULT_COPY_MOVE_OPERATIONS(Scene)

        PC_CORE_API Scene() = default;
        
        PC_CORE_API ~Scene() = default;
    
        EntityRegister entityRegister;
    private:
        

    };

  


END_PCCORE
