#pragma once

#include <string>
#include <vector>

#include "core_header.hpp"
#include "ecs_header.h"

BEGIN_PCCORE

using CreateFunc = void (*)(std::vector<uint8_t>* _compdata, size_t _index, EntityId _entity);
using DeleteFunc = void (*)(void* _ptr);

    struct CreateRegisterComponentInfo
    {
        uint32_t reflectHash = -1;
        std::string name;
    };

class EntityRegister
{
public:
    void RegisterComponent(const CreateRegisterComponentInfo& _CreateRegisterComponentInfo);

private:
    //std::vector<SparseSetsComponent> sparseSets;
};

END_PCCORE