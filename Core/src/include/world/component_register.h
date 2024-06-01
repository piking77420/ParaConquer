﻿#pragma once
#include <map>
#include <vector>

#include "core_header.hpp"
#include "world_header_typedef.hpp"

BEGIN_PCCORE


using CreateFunc = void (*)(std::vector<uint8_t>* _compdata, size_t _index, Entity _entity, void** _ptr);
using DeleteFunc = void (*)(void* _ptr);

class ComponentRegister
{
public:
    template<typename T>
    static uint32_t RegisterComponent(CreateFunc _createFunc, DeleteFunc _deleteFunc);

    template<typename  T>
    static void CreateComponent(std::vector<uint8_t>* _compdata, size_t _index, Entity _entity, void** _ptr);

    static CreateFunc GetCreateFunc(uint32_t _componentId);

    static DeleteFunc GetDeleteFunc(uint32_t _componentId);

    static size_t GetSize(uint32_t _componentId);

    template<class T>
    static void FreeComponent(void* _ptr);

    static uint32_t GetNbrOfComponentType();
    

    struct RegisterComponentFunc
    {
        size_t size;
        CreateFunc createFunc;
        DeleteFunc deleteFunc;
    };
    
    static inline std::map<uint32_t, RegisterComponentFunc>* componentRegisterMap = new std::map<uint32_t, RegisterComponentFunc>();
};

template <typename T>
uint32_t ComponentRegister::RegisterComponent(CreateFunc _createFunc, DeleteFunc _deleteFunc)
{
    uint32_t index = static_cast<uint32_t>(componentRegisterMap->size());
    componentRegisterMap->insert({index , {sizeof(T), _createFunc, _deleteFunc}});

    return index;
}
template<typename  T>
void ComponentRegister::CreateComponent(std::vector<uint8_t>* _compData, size_t _index, Entity _entity, void** _ptr)
{
    T* ptrToNew = new(&_compData[_index])T();
    ptrToNew->componentHolder.entityID = _entity;
}

template <class T>
void ComponentRegister::FreeComponent(void* ptr)
{
    T* component = reinterpret_cast<T*>(ptr);
    component->~T();
}

END_PCCORE