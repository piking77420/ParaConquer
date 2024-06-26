#pragma once
#include <map>
#include <vector>
#include <typeinfo>

#include "core_header.hpp"
#include "world_header_typedef.hpp"
#include "reflection/reflection_typedef.hpp"
//#include "reflected_type.hpp"

BEGIN_PCCORE


using CreateFunc = void (*)(std::vector<uint8_t>* _compdata, size_t _index, Entity _entity);
using DeleteFunc = void (*)(void* _ptr);

class ComponentRegister
{
public:
    template<typename T>
    static uint32_t RegisterComponent(CreateFunc _createFunc, DeleteFunc _deleteFunc);
    
    template<typename  T>
    static void CreateComponent(std::vector<uint8_t>* _compdata, size_t _index, Entity _entity);

    static CreateFunc GetCreateFunc(uint32_t _componentId);

    static DeleteFunc GetDeleteFunc(uint32_t _componentId);

    static size_t GetSize(uint32_t _componentId);

    template<class T>
    static void FreeComponent(void* _ptr);

    static uint32_t GetNbrOfComponentType();
    
    struct RegisterComponentBackend
    {
        size_t size;
        CreateFunc createFunc;
        DeleteFunc deleteFunc;
        const char* name;
        std::vector<ReflectionType> reflecteds;
    };
    
    static inline std::map<uint32_t, RegisterComponentBackend>* componentRegisterMap = new std::map<uint32_t, RegisterComponentBackend>();
};

template <typename T>
uint32_t ComponentRegister::RegisterComponent(CreateFunc _createFunc, DeleteFunc _deleteFunc)
{
    uint32_t index = static_cast<uint32_t>(componentRegisterMap->size());
    componentRegisterMap->insert({index , {sizeof(T), _createFunc, _deleteFunc, typeid(T).name() }});

    return index;
}
template<typename  T>
void ComponentRegister::CreateComponent(std::vector<uint8_t>* _compData, size_t _index, Entity _entity)
{
    T* ptrToNew = new(&_compData->at(_index))T();
    ptrToNew->componentHolder.entityID = _entity;
}

template <class T>
void ComponentRegister::FreeComponent(void* ptr)
{
    T* component = reinterpret_cast<T*>(ptr);
    component->~T();
}


END_PCCORE
