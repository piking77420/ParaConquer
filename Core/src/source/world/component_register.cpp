#include "world/component_register.h"

#include <stdexcept>

#include "log.hpp"

PC_CORE::CreateFunc PC_CORE::ComponentRegister::GetCreateFunc(uint32_t componentId)
{
    if (componentId >= componentRegisterMap->size() || !componentRegisterMap->contains(componentId))
    {
        PC_LOGERROR("componentId is not Valid")
        exit(1);
    }

    return componentRegisterMap->at(componentId).createFunc;
}

PC_CORE::DeleteFunc PC_CORE::ComponentRegister::GetDeleteFunc(uint32_t componentId)
{
    if (componentId >= componentRegisterMap->size() || !componentRegisterMap->contains(componentId))
    {
        PC_LOGERROR("componentId is not Valid")
        throw std::exception("componentId is not Valid");
    }

    return componentRegisterMap->at(componentId).deleteFunc;
}

size_t PC_CORE::ComponentRegister::GetSize(uint32_t componentId)
{
    if (componentId >= componentRegisterMap->size() || !componentRegisterMap->contains(componentId))
    {
        PC_LOGERROR("componentId is not Valid")
        throw std::exception("componentId is not Valid");
    }
    
    return componentRegisterMap->at(componentId).size;
}

uint32_t PC_CORE::ComponentRegister::GetNbrOfComponentType()
{
    return static_cast<uint32_t>(componentRegisterMap->size());
}

std::string PC_CORE::ComponentRegister::GetCorrectComponentName(const std::string& _name)
{
    const size_t firstIndex = _name.find_first_of("::",0);
    std::string out;

    if (firstIndex)
    {
        for (size_t i = firstIndex + 2; i < _name.size(); i++)
            out.push_back(_name[i]); 
    }
    else
    {
        const size_t secondIndex = _name.find_first_of(" ",0);
        
        for (size_t i = secondIndex; i < _name.size(); i++)
            out.push_back(_name[i]); 
    }
    
    return out; 
}
