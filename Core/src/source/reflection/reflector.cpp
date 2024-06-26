#include "reflection/reflector.hpp"
#include "world/component_register.h"

using namespace PC_CORE;

void Reflector::RegisterInternal(uint32_t _componentID, size_t _size, size_t _offset, DataType _datatype, const char* _name)
{
    ComponentRegister::componentRegisterMap->at(_componentID).reflecteds.push_back
    (
        {
            .name = _name,
            .size = _size,
            .offset = _offset,
            .datatype = _datatype
        }
    );
}
