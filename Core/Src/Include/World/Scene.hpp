#pragma once

#include <Array>
#include <Memory>
#include <Vector>
#include <Cassert>

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
    
        
    private:
        

    };

  


END_PCCORE
